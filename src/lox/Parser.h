#pragma once
#include "Token.h"
#include <vector>
#include <array>
#include <cassert>
#include <memory>
#include "Expression.h"
#include "Statement.h"
#include "ErrorManager.h"

namespace pimentel
{
    class Parser
    {
    public:
        Parser(const std::vector<Token>& tokens);
        ~Parser() = default;

        std::vector<std::unique_ptr<Statement>> parse();

    private:

        std::unique_ptr<Statement> doDeclaration(ScopeType scopeType);

        std::unique_ptr<Statement> doVarDecl();
        std::unique_ptr<Statement> doPrintStmt();
        std::unique_ptr<Statement> doExprStmt();

        std::unique_ptr<Statement> doStmt(ScopeType scopeType);
        std::unique_ptr<Statement> doBlockStmt(ScopeType scopeType);
        std::unique_ptr<Statement> doIfStmt(ScopeType scopeType);
        std::unique_ptr<Statement> doWhileStmt(ScopeType scopeType);
        std::unique_ptr<Statement> doForStmt(ScopeType scopeType);
        std::unique_ptr<Statement> doBreakStmt(ScopeType scopeType);

        std::vector<std::unique_ptr<Statement>> doScopeStmts(ScopeType scopeType);

        ExprPtr doExpression();
        ExprPtr doAssignment();
        ExprPtr doEquality();
        ExprPtr doComparison();
        ExprPtr doTerm();
        ExprPtr doFactor();
        ExprPtr doUnary();
        ExprPtr doPrimary();
        ExprPtr doLogical();
        ExprPtr doLogicalOr();
        ExprPtr doLogicalAnd();

        Token consume(TokenType tokenType, const std::string& message);

        void synchronize();

        void error(Token token, const std::string& msg);

        template<typename T>
        bool match(const T& tokenTypes);

        bool check(TokenType type);

        Token peek();

        Token previous();

        bool isAtEnd();

        Token advance();
    
    private:
        std::vector<Token> m_tokens;
        int m_current;
    };
}