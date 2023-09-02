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

        std::vector<StmtPtr> parse();

    private:

        StmtPtr doDeclaration(ScopeType scopeType);

        StmtPtr doVarDecl();
        StmtPtr doPrintStmt();
        StmtPtr doExprStmt();

        StmtPtr doStmt(ScopeType scopeType);
        StmtPtr doFunctionDecl(ScopeType scopeType);
        std::unique_ptr<BlockStmt> doBlockStmt(ScopeType scopeType);
        StmtPtr doIfStmt(ScopeType scopeType);
        StmtPtr doWhileStmt(ScopeType scopeType);
        StmtPtr doForStmt(ScopeType scopeType);
        StmtPtr doBreakStmt(ScopeType scopeType);
        StmtPtr doReturnStmt(ScopeType scopeType);

        std::vector<StmtPtr> doScopeStmts(ScopeType scopeType);

        ExprPtr doExpression();
        ExprPtr doCall();
        ExprPtr doArgumentListAndFinishCall(ExprPtr&& callExpr);
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