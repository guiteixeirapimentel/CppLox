#pragma once
#include <string>

namespace pimentel
{

class Lox
{
public:
    Lox() = default;
    ~Lox() = default;

    void runFile(const std::string& filename);
    void runPrompt();

};

}