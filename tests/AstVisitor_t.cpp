#include <gtest/gtest.h>
#include <lox/AstPrinter.hpp>

using namespace pimentel;

TEST(AstVisitor, BasicPrint)
{
    auto unary = std::unique_ptr<Expression>(new Unary{
            Token{TokenType::MINUS, "-", nullptr, 1},
            std::unique_ptr<Expression>(new Literal{123}) });

    Token token = Token(TokenType::STAR, "*", nullptr, 1);

    auto grouping = std::unique_ptr<Expression>(new Grouping{ std::unique_ptr<Expression>(new Literal{45.67}) });

    auto expression = std::unique_ptr<Expression>(new Binary{ std::move(unary), token, std::move(grouping) });

    AstPrinter astPrinter;

    const auto output = astPrinter.print(*expression);
    const auto expectedOutput = std::string{"(* (- 123.000000) (group 45.670000))"};

    ASSERT_EQ(output, expectedOutput);
}