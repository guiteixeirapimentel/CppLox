#include "Lox.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include "Scanner.h"

#include "ErrorManager.h"

using namespace pimentel;

namespace
{
    void run(const std::string& code)
    {
        Scanner scanner{code};

        const auto tokens = scanner.scanTokens();

        for(const auto& token : tokens)
        {
            std::cout << "token: " << token.toString() << std::endl;
        }
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

    run(dataStream);
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

        run(line);
    }
}