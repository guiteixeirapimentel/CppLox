#include "Parser.h"

using namespace pimentel;

Parser::Parser(const std::vector<Token>& tokens)
    :
    m_tokens(tokens),
    m_current(0)
{}

template<>
bool Parser::match(const TokenType& tokenType)
{
    if (check(tokenType))
    {
        advance();

        return true;
    }

    return false;
}

template<typename T>
bool Parser::match(const T& tokenTypes)
{
    for (const auto& type : tokenTypes)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }

    return false;
}

std::vector<StmtPtr> Parser::parse()
{
    std::vector<StmtPtr> stmts{};

    while (!isAtEnd())
    {
        stmts.emplace_back(doDeclaration(ScopeType::GLOBAL));
    }

    return stmts;
}

StmtPtr Parser::doDeclaration(ScopeType scopeType)
{
    if (match(TokenType::VAR)) return doVarDecl();
    if (match(TokenType::FUN)) return doFunctionDecl(scopeType);

    return doStmt(scopeType);
}

StmtPtr Parser::doVarDecl()
{
    const auto name = advance();

    if (name.getType() != TokenType::IDENTIFIER)
    {
        ErrorManager::get().report(name, "Expected identifier!");
        return {};
    }

    auto initExpr = match(TokenType::EQUAL) ? doExpression() :
        ExprPtr{};

    auto varDecl = std::make_unique<VarStmt>(name, std::move(initExpr));

    consume(TokenType::SEMICOLON, "Expect ';' after var decl.");

    return varDecl;
}

StmtPtr Parser::doStmt(ScopeType scopeType)
{
    if (match(TokenType::PRINT)) return doPrintStmt();
    if (match(TokenType::IF)) return doIfStmt(scopeType);
    if (match(TokenType::WHILE)) return doWhileStmt(scopeType);
    if (match(TokenType::FOR)) return doForStmt(scopeType);
    if (match(TokenType::BREAK)) return doBreakStmt(scopeType);
    if (match(TokenType::RETURN)) return doReturnStmt(scopeType);
    if (match(TokenType::LEFT_BRACE)) return doBlockStmt(scopeType);

    return doExprStmt();
}

StmtPtr Parser::doFunctionDecl(ScopeType scopeType)
{
    const auto newScopeType = (scopeType == ScopeType::FOR_WHILE ||
        scopeType == ScopeType::FOR_WHILE_FUNCTION) ? ScopeType::FOR_WHILE_FUNCTION : ScopeType::FUNCTION;

    const auto name = advance();

    if (name.getType() != TokenType::IDENTIFIER)
    {
        ErrorManager::get().report(name, "Expected identifier!");
        return {};
    }

    if (!match(TokenType::LEFT_PAREN))
    {
        ErrorManager::get().report(name, "Expected opening parenthesis.");
        return {};
    }

    std::vector<Token> argList;

    if (peek().getType() != TokenType::RIGHT_PAREN)
    {
        do
        {
            const auto arg = advance();
            if (arg.getType() != TokenType::IDENTIFIER)
            {
                ErrorManager::get().report(arg, "Expected identifier as argument.");
                return {};
            }
            argList.push_back(arg);
        } while (match(TokenType::COMMA));
    }

    if (!match(TokenType::RIGHT_PAREN))
    {
        ErrorManager::get().report(name, "Expected closing parenthesis.");
        return {};
    }

    if (!match(TokenType::LEFT_BRACE))
    {
        ErrorManager::get().report(name, "Expected function body.");
        return {};
    }

    auto block = doBlockStmt(newScopeType);

    return std::make_unique<FunctionDeclStmt>(name, std::move(block), std::move(argList));
}

StmtPtr Parser::doPrintStmt()
{
    auto val = doExpression();

    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_unique<PrintStmt>(std::move(val));
}

StmtPtr Parser::doExprStmt()
{
    auto val = doExpression();

    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_unique<ExpressionStmt>(std::move(val));
}

std::unique_ptr<BlockStmt> Parser::doBlockStmt(ScopeType scopeType)
{
    return std::make_unique<BlockStmt>(doScopeStmts(scopeType));
}

StmtPtr pimentel::Parser::doIfStmt(ScopeType scopeType)
{
    auto expr = doExpression();
    auto thenBlock = doStmt(scopeType);
    auto elseBlock = StmtPtr{};
    if (check(TokenType::ELSE))
    {
        advance();
        elseBlock = doStmt(scopeType);
    }
    return std::make_unique<IfStmt>(std::move(expr), std::move(thenBlock), std::move(elseBlock));
}

StmtPtr Parser::doWhileStmt(ScopeType scopeType)
{
    const auto newScopeType = (scopeType == ScopeType::FUNCTION ||
        scopeType == ScopeType::FOR_WHILE_FUNCTION) ? ScopeType::FOR_WHILE_FUNCTION : ScopeType::FOR_WHILE;

    auto expr = doExpression();
    auto block = doStmt(newScopeType);

    return std::make_unique<WhileStmt>(std::move(expr), std::move(block));
}

StmtPtr pimentel::Parser::doForStmt(ScopeType scopeType)
{
    const auto newScopeType = (scopeType == ScopeType::FUNCTION ||
        scopeType == ScopeType::FOR_WHILE_FUNCTION) ? ScopeType::FOR_WHILE_FUNCTION : ScopeType::FOR_WHILE;

    consume(TokenType::LEFT_PAREN, "Expected '(' after for.");

    StmtPtr variableDef = nullptr;
    ExprPtr expr = nullptr;
    ExprPtr incExpr = nullptr;

    if (!match(TokenType::SEMICOLON))
    {
        variableDef = doDeclaration(newScopeType);
    }

    if (!match(TokenType::SEMICOLON))
    {
        expr = doExpression();
        consume(TokenType::SEMICOLON, "Expected ';' after expr.");
    }
    if (!match(TokenType::RIGHT_PAREN))
    {
        incExpr = doExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after for.");
    }

    auto block = doStmt(newScopeType);

    return std::make_unique<ForStmt>(std::move(variableDef), std::move(expr), std::move(incExpr), std::move(block));
}

StmtPtr Parser::doBreakStmt(ScopeType scopeType)
{
    if (scopeType != ScopeType::FOR_WHILE &&
        scopeType != ScopeType::FOR_WHILE_FUNCTION)
    {
        ErrorManager::get().report(advance(), "'break' used out of loop stmt.");
        return {};
    }

    consume(TokenType::SEMICOLON, "Expect ';' after break.");
    return std::make_unique<BreakStmt>();
}

StmtPtr Parser::doReturnStmt(ScopeType scopeType)
{
    if (scopeType != ScopeType::FUNCTION &&
        scopeType != ScopeType::FOR_WHILE_FUNCTION)
    {
        ErrorManager::get().report(advance(), "'return' used out of function.");
        return {};
    }

    ExprPtr expr = {};
    if(!match(TokenType::SEMICOLON))
    {
        expr = doExpression();
        consume(TokenType::SEMICOLON, "Expect ';' after return stmt.");
    }

    return std::make_unique<ReturnStmt>(std::move(expr));
}

std::vector<StmtPtr> Parser::doScopeStmts(ScopeType scopeType)
{
    std::vector<StmtPtr> res;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        res.push_back(doDeclaration(scopeType));
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");

    return res;
}

ExprPtr Parser::doExpression()
{
    return doAssignment();
}

ExprPtr Parser::doCall()
{
    ExprPtr expr = doPrimary();

    while (true)
    {
        if (match(TokenType::LEFT_PAREN))
        {
            expr = doArgumentListAndFinishCall(std::move(expr));
        }
        else
        {
            break;
        }
    }

    return expr;
}

ExprPtr pimentel::Parser::doIndexing()
{
    ExprPtr expr = doCall();

    if(match(TokenType::LEFT_SQR_BRACKET))
    {
        auto index = doExpression();

        consume(TokenType::RIGHT_SQR_BRACKET, "Expected ']' after indexing.");
        return std::make_unique<Indexing>(std::move(expr), previous(), std::move(index));
    }

    return expr;
}

ExprPtr pimentel::Parser::doArgumentListAndFinishCall(ExprPtr&& callExpr)
{
    std::vector<ExprPtr> argList;

    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (argList.size() > 255)
            {
                error(peek(), "Can't have more than 255 arguments.");
            }
            argList.emplace_back(doExpression());
        } while (match(TokenType::COMMA));

    }

    auto paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments on call.");

    return std::make_unique<Call>(std::move(callExpr), paren, std::move(argList));
}

ExprPtr Parser::doAssignment()
{
    auto expr = doLogical();

    if (match(TokenType::EQUAL))
    {
        auto val = doAssignment();

        const auto exprAsVar = dynamic_cast<Variable*>(expr.get());

        if (exprAsVar)
        {
            auto name = exprAsVar->name;
            return std::make_unique<Assignment>(name, std::move(val));
        }

        const auto equals = previous();
        error(equals, "Invalid assignment target!");
    }

    return expr;
}

ExprPtr Parser::doEquality()
{
    auto expr = doComparison();

    while (match(std::array{ TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL }))
    {
        Token op = previous();
        auto right = doComparison();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

ExprPtr Parser::doComparison()
{
    auto expr = doTerm();

    while (match(std::array{ TokenType::GREATER,
        TokenType::GREATER_EQUAL,TokenType::LESS,TokenType::LESS_EQUAL }))
    {
        Token op = previous();
        auto right = doTerm();

        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

ExprPtr Parser::doTerm()
{
    auto expr = doFactor();

    while (match(std::array{ TokenType::MINUS, TokenType::PLUS }))
    {
        Token op = previous();
        auto right = doFactor();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

ExprPtr Parser::doFactor()
{
    auto expr = doUnary();

    while (match(std::array{ TokenType::SLASH, TokenType::STAR }))
    {
        auto op = previous();
        auto right = doUnary();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

ExprPtr Parser::doUnary()
{
    if (match(std::array{ TokenType::BANG, TokenType::MINUS }))
    {
        auto op = previous();
        auto right = doUnary();
        return std::make_unique<Unary>(op, std::move(right));
    }

    return doIndexing();
}

ExprPtr Parser::doPrimary()
{
    if (match(TokenType::FALSE)) return std::make_unique<Literal>(false);
    if (match(TokenType::TRUE)) return std::make_unique<Literal>(true);
    if (match(TokenType::NIL)) return std::make_unique<Literal>(nullptr);

    if (match(std::array{ TokenType::NUMBER, TokenType::STRING }))
    {
        return std::make_unique<Literal>(previous().getLiteral());
    }

    if (match(TokenType::LEFT_PAREN))
    {
        auto expr = doExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }

    if (match(TokenType::IDENTIFIER))
    {
        return std::make_unique<Variable>(previous());
    }

    ErrorManager::get().report(peek(), "Expected expression.");

    // assert(0);
    return nullptr;
}

ExprPtr pimentel::Parser::doLogical()
{
    return doLogicalOr();
}

ExprPtr pimentel::Parser::doLogicalOr()
{
    auto expr = doLogicalAnd();

    while (match(TokenType::OR))
    {
        Token op = previous();

        auto right = doLogicalAnd();

        expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
    }

    return expr;
}

ExprPtr pimentel::Parser::doLogicalAnd()
{
    auto expr = doEquality();

    while (match(TokenType::AND))
    {
        Token op = previous();

        auto right = doEquality();

        expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
    }

    return expr;
}

Token Parser::consume(TokenType tokenType, const std::string& message)
{
    if (check(tokenType)) return advance();

    error(previous(), message);

    // assert(0);
    return {};
}

void Parser::synchronize()
{
    advance();
    while (!isAtEnd())
    {
        if (previous().getType() == TokenType::SEMICOLON) return;

        switch (peek().getType())
        {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
            return;
            break;
        default:
            advance();
            break;
        }
    }
}

void Parser::error(Token token, const std::string& msg)
{
    ErrorManager::get().report(token, msg);
}

bool Parser::check(TokenType type)
{
    if (isAtEnd())
        return false;

    return peek().getType() == type;
}

Token Parser::peek()
{
    return m_tokens[m_current];
}

Token Parser::previous()
{
    return m_tokens[m_current - 1];
}

bool Parser::isAtEnd()
{
    return peek().getType() == TokenType::ENDOFFILE;
}

Token Parser::advance()
{
    if (!isAtEnd()) m_current++;
    return previous();
}