#pragma once
#include <sstream>
#include <vector>

#include "Token.h"

namespace pimentel
{
    class Scanner
    {
    public:
        Scanner(const std::string& code);
        ~Scanner() = default;

        std::vector<Token> scanTokens();

    private:
        bool isAtEnd() const;
        void scanToken(std::vector<Token>& out);
        unsigned char advance();
        Token getToken(TokenType token);
        Token getToken(TokenType token, Token::LiteralType literal);

        bool match(char expected);
        char peek();

        void string(std::vector<Token>& out);

    private:
        std::string m_code;
        int m_line;
        int m_start;
        int m_current;
    };
}