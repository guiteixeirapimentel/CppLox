#include "ErrorManager.h"
#include "Token.h"

#include <iostream>

#include <memory>

using namespace pimentel;

void ErrorManager::report(const Token& token, const std::string& message)
{
    if(token.getType() == TokenType::ENDOFFILE)
    {
        report(token.getLine(), " at end", message);
        return;
    }

    report(token.getLine(), " at '" + token.getLexeme() + "'", message);
}

void ErrorManager::report(int line, const std::string& where, const std::string& message)
{
    std::cout << "[line " << line << "] Error" << where << ": " << message << std::endl;

    m_hasError = true;
}

void ErrorManager::report(int line, const std::string& message)
{
    report(line, "", message);
}

bool ErrorManager::hasError() const
{
    return m_hasError;
}

void ErrorManager::resetError()
{
    m_hasError = false;
}

ErrorManager& ErrorManager::get()
{
    static std::unique_ptr<ErrorManager> pInstance = nullptr;

    if(!pInstance)
    {
        pInstance = std::unique_ptr<ErrorManager>{new ErrorManager{}};
    }

    return *pInstance;
}
