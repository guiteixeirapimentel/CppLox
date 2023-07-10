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
        std::unique_ptr<Statement> doStmt();
        std::unique_ptr<Statement> doPrintStmt();
        std::unique_ptr<Statement> doExprStmt();

        std::unique_ptr<Expression> doExpression();
        std::unique_ptr<Expression> doEquality();
        std::unique_ptr<Expression> doComparison();
        std::unique_ptr<Expression> doTerm();
        std::unique_ptr<Expression> doFactor();
        std::unique_ptr<Expression> doUnary();
        std::unique_ptr<Expression> doPrimary();

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