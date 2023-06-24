#include <iostream>
#include <lox/Lox.h>

#include <lox/ErrorManager.h>

int main(int argc, char** argv)
{
    if(argc > 2)
    {
        std::cout << "Usage: cpplox [script]" << std::endl;
        return 64;
    }

    pimentel::Lox lox{};

    if(argc == 2)
    {
        lox.runFile(argv[1]);    
        return 0;    
    }

    lox.runPrompt();

    const auto& errorMngr = pimentel::ErrorManager::get();
    if(errorMngr.hasError())
    {
        return -1;
    }

    return 0;
}