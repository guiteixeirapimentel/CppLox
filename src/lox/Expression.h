#pragma once
#include "Token.h"
#include "ExpressionVisitor.hpp"
#include "VisitorUtils.hpp"
#include <memory>
#include <vector>

namespace pimentel
{
    struct Expression 
    {
        Expression() = default;
        virtual ~Expression() = default;

        virtual ExprVisitorString::RetType accept(ExprVisitorString& visitor) = 0;
        virtual ExprVisitorLoxVal::RetType accept(ExprVisitorLoxVal& visitor) = 0;
    };

    using ExprPtr = std::unique_ptr<Expression>;

    struct Binary : public Expression
    {
        Binary() = default;
        Binary(ExprPtr left, Token operatorType, ExprPtr right)
            :
            left(std::move(left)), operatorType(operatorType), right(std::move(right)){}

        ExprPtr left;
        Token operatorType;
        ExprPtr right;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);
    };

    struct Grouping : public Expression
    {
        Grouping() = default;
        Grouping(ExprPtr expr) : expr(std::move(expr)){};
        ExprPtr expr;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);
    };

    struct Literal : public Expression
    {
        Literal(double val)
            :
            value{val}
        {}

        Literal(int val)
            :
            value((double)val)
        {}

        Literal(bool val)
            :
            value(val)
        {}

        Literal(Token::LiteralType val)
            :
            value(val)
        {}

        Literal()
            :
            value(nullptr)
        {}

        Token::LiteralType value;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);
    };

    struct Unary : public Expression
    {
        Unary() = default;
        Unary(Token operatorType, ExprPtr right)
            :
            operatorType (operatorType), right(std::move(right)){};

        Token operatorType;
        ExprPtr right;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);
    };

    struct Variable : public Expression
    {
        Variable() = default;
        Variable(const Token& name) : name(name) {};

        Token name;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);
    };

    struct Assignment : public Expression
    {
        Assignment() = default;
        Assignment(Token name, ExprPtr value)
            :
            name(name), value(std::move(value))
        {}

        Token name;
        ExprPtr value;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);
    };

    struct Logical : public Expression
    {
        Logical() = default;
        Logical(ExprPtr leftExpr, Token op, ExprPtr rightExpr)
            :
            leftExpr(std::move(leftExpr)),
            op(op),
            rightExpr(std::move(rightExpr))
        {}
        ~Logical() = default;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);

        ExprPtr leftExpr;
        Token op;
        ExprPtr rightExpr;
    };

    struct Call : public Expression
    {
        Call() = default;
        Call(ExprPtr calee, Token paren, std::vector<ExprPtr>&& arguments)
            :
            calee(std::move(calee)),
            paren(paren),
            arguments(std::move(arguments))
        {}
        ~Call() = default;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);

        ExprPtr calee;
        Token paren;
        std::vector<ExprPtr> arguments;
    };

    struct Indexing : public Expression
    {
        Indexing() = default;
        Indexing(ExprPtr indexee, Token brackets, ExprPtr index)
            :
            indexee(std::move(indexee)),
            brackets(brackets),
            index(std::move(index))
        {}
        ~Indexing() = default;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);

        ExprPtr indexee;
        Token brackets;
        ExprPtr index;
    };
}