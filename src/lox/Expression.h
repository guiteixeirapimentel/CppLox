#pragma once
#include "Token.h"
#include "ExpressionVisitor.hpp"
#include "VisitorUtils.hpp"
#include <memory>

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
        Assignment(Token name, std::unique_ptr<Expression> value)
            :
            name(name), value(std::move(value))
        {}

        Token name;
        std::unique_ptr<Expression> value;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);
    };

    struct Logical : public Expression
    {
        Logical() = default;
        Logical(std::unique_ptr<Expression> leftExpr, Token op, std::unique_ptr<Expression> rightExpr)
            :
            leftExpr(std::move(leftExpr)),
            op(op),
            rightExpr(std::move(rightExpr))
        {}
        ~Logical() = default;

        ACCEPT_IMPL(ExprVisitorString);
        ACCEPT_IMPL(ExprVisitorLoxVal);

        std::unique_ptr<Expression> leftExpr;
        Token op;
        std::unique_ptr<Expression> rightExpr;
    };
}