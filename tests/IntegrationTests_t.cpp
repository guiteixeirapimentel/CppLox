#include <gtest/gtest.h>

#include <memory>

#include <lox/Scanner.h>
#include <lox/Parser.h>
#include <lox/Interpreter.h>

using namespace pimentel;

class BasicIntegrationFixture : 
    public ::testing::TestWithParam<std::tuple<std::string, std::string>>
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}

    void runCode(const std::string& code)
    {
        Scanner scanner{code};

        const auto tokens = scanner.scanTokens();

        Parser p{tokens};

        // AstPrinter astPrinter;

        const auto stmts = p.parse();

        if(!stmts.size() || ErrorManager::get().hasError())
        {
            std::cout << "Errors found, please fix." << std::endl;

            return;
        }

        // std::cout << astPrinter.print(*stmts) << std::endl;
        m_interpreter.interpret(stmts);
    }
    
    std::stringstream outStream;
    Interpreter m_interpreter{outStream};
};

TEST_P(BasicIntegrationFixture, PrintTest)
{
    const auto [code, expectedOutput] = GetParam();
    runCode(code);

    EXPECT_EQ(outStream.str(), expectedOutput);
}

static const auto testParams = std::vector{
    std::tuple{std::string{"print(1);"}, std::string{"1.000000\n"}},
    std::tuple{
        std::string{"var i = 0; while(i < 10) { print(i); i = i + 1;} print(i);"},
        std::string{"0.000000\n1.000000\n2.000000\n3.000000\n4.000000\n5.000000\n6.000000\n7.000000\n8.000000\n9.000000\n10.000000\n"}
    },
    std::tuple{
        std::string{"var i = 0; while(i < 10) { if(i>5) break; print(i); i = i + 1;} print(i);"},
        std::string{"0.000000\n1.000000\n2.000000\n3.000000\n4.000000\n5.000000\n6.000000\n"}
    },
    std::tuple{
        std::string{"var i = 0; if(i) { print(true);} else { print(false);}"},
        std::string{"false\n"}
    },
    std::tuple{
        std::string{"var i = -1; if(i) { print(true);} else { print(false);}"},
        std::string{"true\n"}
    },
    std::tuple{
        std::string{"print((1 + 2) + 4 / 552);"},
        std::string{"3.007246\n"}
    },
    std::tuple{
        std::string{"print(\"a\" + \"b\" + \"...\");"},
        std::string{"ab...\n"}
    },
    std::tuple{
        std::string{"while(1) { while(1) { break; } print(1); break;} print(\"finished\");"},
        std::string{"1.000000\nfinished\n"}
    },
};

INSTANTIATE_TEST_SUITE_P(BasicNumberTest, BasicIntegrationFixture,
    ::testing::ValuesIn(testParams));