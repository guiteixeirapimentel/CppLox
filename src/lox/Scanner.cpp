#include "Scanner.h"
#include "ErrorManager.h"

#include <cassert>
#include <unordered_map>

using namespace pimentel;

namespace
{
    const std::unordered_map<std::string, TokenType> reservedKeywords = {
        { "and" , TokenType::AND },
        { "class" , TokenType::CLASS },
        { "else",   TokenType::ELSE },
        { "false",  TokenType::FALSE },
        { "for",    TokenType::FOR },
        { "fun",    TokenType::FUN },
        { "if",     TokenType::IF },
        { "nil",    TokenType::NIL },
        { "or",     TokenType::OR },
        { "print",  TokenType::PRINT },
        { "return", TokenType::RETURN },
        { "super",  TokenType::SUPER },
        { "this",   TokenType::THIS },
        { "true",   TokenType::TRUE },
        { "var",    TokenType::VAR },
        { "while",  TokenType::WHILE },
        { "break", TokenType::BREAK },
    };

    bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool isAlpha(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
    }
    bool isAlphaNumeric(char c)
    {
        return isAlpha(c) || isDigit(c);
    }
}

Scanner::Scanner(const std::string& code)
    :
    m_code(code)
{}

bool Scanner::isAtEnd() const
{
    return m_current >= static_cast<int>(m_code.length());
}

unsigned char Scanner::advance()
{
    return m_code[m_current++];
}

Token Scanner::getToken(TokenType type)
{
    return getToken(type, {});
}

Token Scanner::getToken(TokenType type, Token::LiteralType literal)
{
    return Token{type, getCurrentLexeme(), literal, m_line};
}

std::string Scanner::getCurrentLexeme() const
{
    const auto nChars = m_current - m_start;
    return m_code.substr(m_start, nChars);
}


void Scanner::scanToken(std::vector<Token>& out)
{
    const char c = advance();

    const auto addToken = [this, &out](TokenType type) {
        out.emplace_back(getToken(type));
    };

    switch(c)
    {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(TokenType::MINUS); break;
        case '+': addToken(TokenType::PLUS); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(TokenType::STAR); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '/':
            if(match('/'))
            {
                while(peek() != '\n' && !isAtEnd())
                {
                    advance();
                }
            }
            else
            {
                addToken(TokenType::SLASH);
            }
        break;
        case '"': string(out); break;
        case '\r':
        case ' ':
        case '\t':
            // skipping whitespace
        break;
        case '\n':
            m_line++;
        break;
        default:
            if(isDigit(c))
            {
                number(out);
            }
            else if(isAlpha(c))
            {
                identifier(out);
            }
            else
            {
                ErrorManager::get()
                    .report(m_line, std::string{"Unexpected character: '"} + c + "'");
            }
        break;
    }
}

void Scanner::number(std::vector<Token>& out)
{
    while(isDigit(peek()))
    {
        advance();
    }

    if(peek() != '.' || !isDigit(peekNext()))
    {
        const auto numberStr = m_code.substr(m_start, m_current);

        const auto val = std::atof(numberStr.c_str());

        out.emplace_back(getToken(TokenType::NUMBER, val));
        return;
    }

    advance();

    while(isDigit(peek()))
    {
        advance();
    }

    const auto numberStr = m_code.substr(m_start, m_current);

    const auto val = std::atof(numberStr.c_str());

    out.emplace_back(getToken(TokenType::NUMBER, val));
}

void Scanner::string(std::vector<Token>& out)
{
    while(peek() != '"' && !isAtEnd())
    {
        if(peek() == '\n') m_line++;

        advance();
    }

    if(isAtEnd())
    {
        ErrorManager::get().report(m_line, "Missing ending \" for string!");
        return;
    }

    assert(advance() == '\"');

    const auto len = (m_current-1) - (m_start+1);
    out.emplace_back(getToken(TokenType::STRING, m_code.substr(m_start+1, len)));
}

void Scanner::identifier(std::vector<Token>& out)
{
    while(isAlphaNumeric(peek()))
    {
        advance();
    }

    auto lexeme = getCurrentLexeme();

    const auto it = reservedKeywords.find(lexeme);

    if(it == reservedKeywords.end())
    {
        out.emplace_back(getToken(TokenType::IDENTIFIER));
        return;
    }

    out.emplace_back(getToken(it->second));
}

bool Scanner::match(char expected)
{
    if(isAtEnd())
        return false;

    if(m_code[m_current] != expected)
        return false;

    m_current++;

    return true;
}

char Scanner::peek()
{
    if(isAtEnd())
        return '\0';

    return m_code[m_current];
}

char Scanner::peekNext()
{
    if(m_current + 1 >= static_cast<int>(m_code.length()))
        return '\0';

    return m_code[m_current + 1];
}

std::vector<Token> Scanner::scanTokens()
{
    std::vector<Token> tokens;

    m_start = 0;
    m_current = 0;
    m_line = 1;

    while(!isAtEnd())
    {
        m_start = m_current;
        scanToken(tokens);
    }

    tokens.push_back(Token{TokenType::ENDOFFILE, "", 0.0, m_line});

    return tokens;
}