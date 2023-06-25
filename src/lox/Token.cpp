#include "Token.h"

#include <sstream>

using namespace pimentel;

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace
{
    std::string tokenTypeToString(TokenType tokenType)
    {
        switch(tokenType)
        {
        case TokenType::LEFT_PAREN:
            return {"LEFT_PAREN"};
        case TokenType::RIGHT_PAREN:
            return {"RIGHT_PAREN"};
        case TokenType::LEFT_BRACE:
            return {"LEFT_BRACE"};
        case TokenType::RIGHT_BRACE:
            return {"RIGHT_BRACE"};
        case TokenType::COMMA:
            return {"COMMA"};
        case TokenType::DOT:
            return {"DOT"};
        case TokenType::MINUS:
            return {"MINUS"};
        case TokenType::PLUS:
            return {"PLUS"};
        case TokenType::SEMICOLON:
            return {"SEMICOLON"};
        case TokenType::SLASH:
            return {"SLASH"};
        case TokenType::STAR:
            return {"STAR"};
        case TokenType::BANG:
            return {"BANG"};
        case TokenType::BANG_EQUAL:
            return {"BANG_EQUAL"};
        case TokenType::EQUAL:
            return {"EQUAL"};
        case TokenType::EQUAL_EQUAL:
            return {"EQUAL_EQUAL"};
        case TokenType::GREATER:
            return {"GREATER"};
        case TokenType::GREATER_EQUAL:
            return {"GREATER_EQUAL"};
        case TokenType::LESS:
            return {"LESS"};
        case TokenType::LESS_EQUAL:
            return {"LESS_EQUAL"};
        case TokenType::IDENTIFIER:
            return {"IDENTIFIER"};
        case TokenType::STRING:
            return {"STRING"};
        case TokenType::NUMBER:
            return {"NUMBER"};
        case TokenType::AND:
            return {"AND"};
        case TokenType::CLASS:
            return {"CLASS"};
        case TokenType::ELSE:
            return {"ELSE"};
        case TokenType::FALSE:
            return {"FALSE"};
        case TokenType::FUN:
            return {"FUN"};
        case TokenType::FOR:
            return {"FOR"};
        case TokenType::IF:
            return {"IF"};
        case TokenType::NIL:
            return {"NIL"};
        case TokenType::OR:
            return {"OR"};
        case TokenType::PRINT:
            return {"PRINT"};
        case TokenType::RETURN:
            return {"RETURN"};
        case TokenType::SUPER:
            return {"SUPER"};
        case TokenType::THIS:
            return {"THIS"};
        case TokenType::TRUE:
            return {"TRUE"};
        case TokenType::VAR:
            return {"VAR"};
        case TokenType::WHILE:
            return {"WHILE"};
        case TokenType::ENDOFFILE:
            return {"ENDOFFILE"};
        }

        return {""};
    }

    std::string literalToString(Token::LiteralType literal)
    {
        return std::visit(overloaded{
            [](const std::string& arg) { return arg; },
            [](const auto& arg) { return std::to_string(arg); },
        }, literal);
    }
}

Token::Token(TokenType type, const std::string& lexeme, LiteralType literal, int line)
    :
    m_type(type),
    m_lexeme(lexeme),
    m_literal(literal),
    m_line(line)
{}

std::string Token::toString() const
{
    std::stringstream res;
    res << tokenTypeToString(m_type) << " lexeme: " << m_lexeme << " literal: " << literalToString(m_literal);

    return res.str();
}
