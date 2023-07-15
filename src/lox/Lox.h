#pragma once
#include <string>
#include "Interpreter.h"

namespace pimentel
{

class Lox
{
public:
    Lox();
    ~Lox() = default;

    void runFile(const std::string& filename);
    void runPrompt();
private:
    Interpreter m_interpreter;

};

}