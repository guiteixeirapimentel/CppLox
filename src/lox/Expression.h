#pragma once
#include "Token.h"
#include "ExpressionVisitor.hpp"
#include <memory>

// I really would preffer not using macros, but as I can't use
// templated virtual functions, this is the best solution to reduce code
// duplication.
#define ACCEPT_IMPL(type) \
type::RetType accept(type& visitor) override \
{ \
    return visitor.visit(*this);\
}

namespace pimentel
{
    struct Expression 
    {
        Expression() = default;
        virtual ~Expression() = default;

        virtual ExprVisitorString::RetType accept(ExprVisitorString& visitor) = 0;
        virtual ExprVisitorLoxVal::RetType accept(ExprVisitorLoxVal& visitor) = 0;
    };

    struct Binary : public Expression
    {
        Binary() = default;
        Binary(std::unique_ptr<Expression> left, Token operatorType, std::unique_ptr<Expression> right)
            :
            left(std::move(left)), operatorType(operatorType), right(std::move(right)){}

        std::unique_ptr<Expression> left;
        Token operatorType;
        std::unique_ptr<Expression> right;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);
    };

    struct Grouping : public Expression
    {
        Grouping() = default;
        Grouping(std::unique_ptr<Expression> expr) : expr(std::move(expr)){};
        std::unique_ptr<Expression> expr;

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
        Unary(Token operatorType, std::unique_ptr<Expression> right)
            :
            operatorType (operatorType), right(std::move(right)){};

        Token operatorType;
        std::unique_ptr<Expression> right;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);
    };
}