#pragma once

#include <string>
#include <variant>

namespace pimentel
{
    enum class TokenType {
        // Single-character tokens.
        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
        LEFT_SQR_BRACKET, RIGHT_SQR_BRACKET,

        // One or two character tokens.
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        // Literals.
        IDENTIFIER, STRING, NUMBER,

        // Keywords.
        AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
        PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE, BREAK,

        ENDOFFILE
    };

    class Token
    {
    public:
        using LiteralType = std::variant<void*, double, std::string, bool>;
    public:
        Token() = default;
        ~Token() = default;

        Token(TokenType type, const std::string& lexeme, LiteralType literal, int line);

        std::string toString() const;

        std::string getLexeme() const;
        TokenType getType() const;
        LiteralType getLiteral() const;
        int getLine() const;

    private:
        TokenType m_type;
        std::string m_lexeme;
        LiteralType m_literal = nullptr;
        int m_line;        
    };
} // namespace pimentel
