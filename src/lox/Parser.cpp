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
    if(check(tokenType))
    {
        advance();

        return true;
    }

    return false;
}

template<typename T>
bool Parser::match(const T& tokenTypes)
{
    for(const auto& type : tokenTypes)
    {
        if(check(type))
        {
            advance();
            return true;
        }
    }

    return false;
}

std::vector<std::unique_ptr<Statement>> Parser::parse()
{
    std::vector<std::unique_ptr<Statement>> stmts{};

    while(!isAtEnd())
    {
        stmts.emplace_back(doDeclaration());
    }
    
    return stmts;
}

std::unique_ptr<Statement> Parser::doDeclaration()
{
    if(match(TokenType::VAR)) return doVarDecl();

    return doStmt();
}

std::unique_ptr<Statement> Parser::doVarDecl()
{
    const auto name = advance();

    if(name.getType() != TokenType::IDENTIFIER)
    {
        ErrorManager::get().report(name, "Expected identifier!");
        return {};
    }

    auto initExpr = match(TokenType::EQUAL) ? doExpression() :
        std::unique_ptr<Expression>{};

    auto varDecl = std::make_unique<VarStmt>(name, std::move(initExpr));

    consume(TokenType::SEMICOLON, "Expect ';' after var decl.");

    return varDecl;
}

std::unique_ptr<Statement> Parser::doStmt()
{
    if(match(TokenType::PRINT)) return doPrintStmt();

    return doExprStmt();
}

std::unique_ptr<Statement> Parser::doPrintStmt()
{
    auto val = doExpression();

    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_unique<PrintStmt>(std::move(val));
}

std::unique_ptr<Statement> Parser::doExprStmt()
{
    auto val = doExpression();

    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_unique<ExpressionStmt>(std::move(val));
}

std::unique_ptr<Expression> Parser::doExpression()
{
    return doAssignment();
}

std::unique_ptr<Expression> Parser::doAssignment()
{
    auto expr = doEquality();

    if(match(TokenType::EQUAL))
    {
        auto val = doAssignment();

        const auto exprAsVar = dynamic_cast<Variable*>(expr.get());

        if(exprAsVar)
        {
            auto name = exprAsVar->name;
            return std::make_unique<Assignment>(name, std::move(val));
        }

        const auto equals = previous();
        error(equals, "Invalid assignment target!");
    }

    return expr;
}

std::unique_ptr<Expression> Parser::doEquality()
{
    auto expr = doComparison();

    while(match(std::array{TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
        Token op = previous();
        auto right = doComparison();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::doComparison()
{
    auto expr = doTerm();

    while(match(std::array{TokenType::GREATER,
        TokenType::GREATER_EQUAL,TokenType::LESS,TokenType::LESS_EQUAL}))
    {
        Token op = previous();
        auto right = doTerm();

        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::doTerm()
{
    auto expr = doFactor();

    while(match(std::array{TokenType::MINUS, TokenType::PLUS}))
    {
        Token op = previous();
        auto right = doFactor();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::doFactor()
{
    auto expr = doUnary();

    while(match(std::array{TokenType::SLASH, TokenType::STAR}))
    {
        auto op = previous();
        auto right = doUnary();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::doUnary()
{
    if(match(std::array{TokenType::BANG, TokenType::MINUS}))
    {
        auto op = previous();
        auto right = doUnary();
        return std::make_unique<Unary>(op, std::move(right));
    }

    return doPrimary();
}

std::unique_ptr<Expression> Parser::doPrimary()
{
    if(match(TokenType::FALSE)) return std::make_unique<Literal>(false);
    if(match(TokenType::TRUE)) return std::make_unique<Literal>(true);
    if(match(TokenType::NIL)) return std::make_unique<Literal>(nullptr);

    if(match(std::array{TokenType::NUMBER, TokenType::STRING}))
    {
        return std::make_unique<Literal>(previous().getLiteral());
    }

    if(match(TokenType::LEFT_PAREN))
    {
        auto expr = doExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }

    if(match(TokenType::IDENTIFIER))
    {
        return std::make_unique<Variable>(previous());
    }

    ErrorManager::get().report(peek(), "Expected expression.");
    
    // assert(0);
    return nullptr;
}

Token Parser::consume(TokenType tokenType, const std::string& message)
{
    if(check(tokenType)) return advance();

    error(previous(), message);

    // assert(0);
    return {};
}

void Parser::synchronize()
{
    advance();
    while(!isAtEnd())
    {
        if(previous().getType() == TokenType::SEMICOLON) return;

        switch(peek().getType())
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
    if(isAtEnd())
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
    if(!isAtEnd()) m_current++;
    return previous();
}