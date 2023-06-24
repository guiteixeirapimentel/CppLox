#include "Scanner.h"
#include "ErrorManager.h"

#include <cassert>

using namespace pimentel;

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
    return Token{type, m_code.substr(m_start, m_current), literal, m_line};
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
            ErrorManager::get()
                .report(m_line, std::string{"Unexpected character: '"} + c + "'");
        break;
    }
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

    out.emplace_back(getToken(TokenType::STRING, m_code.substr(m_start+1, m_current-2)));
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

    tokens.push_back(Token{TokenType::ENDOFFILE, "", 0, m_line});

    return tokens;
}