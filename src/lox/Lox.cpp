#include "Lox.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include "ExpressionVisitor.hpp"

#include "Scanner.h"

#include "ErrorManager.h"

#include "Parser.h"
#include "AstPrinter.hpp"

using namespace pimentel;

namespace
{
    void run(const std::string& code, Interpreter& interpreter)
    {
        Scanner scanner{code};

        const auto tokens = scanner.scanTokens();

        Parser p{tokens};

        AstPrinter astPrinter;

        const auto expr = p.parse();

        if(!expr || ErrorManager::get().hasError())
        {
            std::cout << "Errors found, please fix." << std::endl;

            return;
        }

        // std::cout << astPrinter.print(*expr) << std::endl;
        interpreter.interpret(*expr);
    }

    std::string readAllTextFromFile(const std::string& filename)
    {
        std::ifstream file{filename, std::ios::binary | std::ios::ate};

        if(!file.is_open())
        {
            std::cout << "[LOG] Could not find file " << filename << std::endl;
            return {};
        }

        const auto fileSize = file.tellg();
        std::string data;

        data.resize(fileSize);

        file.seekg(0);

        file.read(&data[0], fileSize);

        return data;
    }
}

void Lox::runFile(const std::string& filename)
{
    auto dataStream = readAllTextFromFile(filename);

    run(dataStream, m_interpreter);
}

void Lox::runPrompt()
{
    std::string line{};

    while(true)
    {
        std::cout << "> ";

        std::getline(std::cin, line);

        if(!line.size() || std::cin.eof())
        {
            std::cout << "Ctrl + D" << std::endl;
            break;
        }

        run(line, m_interpreter);

        ErrorManager::get().resetError();
    }
}