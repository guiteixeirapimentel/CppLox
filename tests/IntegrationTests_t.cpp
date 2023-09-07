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
    std::tuple{
        std::string{"print (\"hi\" or 2);"},
        std::string{"true\n"}
    },
    std::tuple{
        std::string{"print (\"hi\" and false);"},
        std::string{"false\n"}
    },
    std::tuple{
        std::string{"var a = 0; var b; (a == 1) and (b = 10); print(b);"},
        std::string{"[Lox obj] = 0\n"}
    },
    std::tuple{
        std::string{"var a = 0; var b; (a == 0) and (b = 10); print(b);"},
        std::string{"10.000000\n"}
    },
    std::tuple{
        std::string{"var i = 20; for(var i = 0; i < 5; i = i + 1) { print(i);} print(i);"},
        std::string{"0.000000\n1.000000\n2.000000\n3.000000\n4.000000\n20.000000\n"}
    },
    std::tuple{
        std::string{"var i = 0; for(;;) { print(i); i = i + 1; if(i>5) break; }"},
        std::string{"0.000000\n1.000000\n2.000000\n3.000000\n4.000000\n5.000000\n"}
    },
    std::tuple{
        std::string{"var i = 10; for(var i = 0;;) { print(i); i = i + 1; if(i>5) break; }"},
        std::string{"0.000000\n1.000000\n2.000000\n3.000000\n4.000000\n5.000000\n"}
    },
    std::tuple{
        std::string{"var i = 0; for(;i < 6;) { print(i); i = i + 1; }"},
        std::string{"0.000000\n1.000000\n2.000000\n3.000000\n4.000000\n5.000000\n"}
    },
    std::tuple{
        std::string{"var i = 0; for(;; i = i + 1) { if(!(i < 6)) break; print(i); }"},
        std::string{"0.000000\n1.000000\n2.000000\n3.000000\n4.000000\n5.000000\n"}
    },
    std::tuple{
        std::string{"var t1 = clock(); print(t1 - t1);"},
        std::string{"0.000000\n"}
    },
    std::tuple{
        std::string{"var t1 = clock(); var t2 = clock(); print(t2 >= t1);"},
        std::string{"true\n"}
    },
    std::tuple{
        std::string{"fun fac(n) { if(n > 1) { return (fac(n-1)*n); } return 1; }"
        "print(fac(10));"},
        std::string{"3628800.000000\n"}
    },
    std::tuple{
        std::string{"var i = 0; fun globVar() { print(i); }"
        "print(i); i = i + 1; print(i);"},
        std::string{"0.000000\n1.000000\n"}
    },

    std::tuple{
        std::string{R"STR(fun makeCounter() {
                    var i = 0;
                    fun count() {
                        i = i + 1;
                        print i;
                    }

                    return count;
                    }

                    var counter = makeCounter();
                    counter(); // "1".
                    counter(); // "2".)STR"},
        std::string{"1.000000\n2.000000\n"},
    },
};

INSTANTIATE_TEST_SUITE_P(BasicNumberTest, BasicIntegrationFixture,
    ::testing::ValuesIn(testParams));