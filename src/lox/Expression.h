#pragma once
#include "Token.h"
#include "ExpressionVisitor.hpp"
#include <memory>

namespace pimentel
{
    struct Expression 
    {
        Expression() = default;
        virtual ~Expression() = default;

        virtual ExprVisitorString::RetType accept(ExprVisitorString& visitor) = 0;
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

        ExprVisitorString::RetType accept(ExprVisitorString& visitor) override
        {
            return visitor.visit(*this);
        }
    };

    struct Grouping : public Expression
    {
        Grouping() = default;
        Grouping(std::unique_ptr<Expression> expr) : expr(std::move(expr)){};
        std::unique_ptr<Expression> expr;

        ExprVisitorString::RetType accept(ExprVisitorString& visitor) override
        {
            return visitor.visit(*this);
        }
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

        ExprVisitorString::RetType accept(ExprVisitorString& visitor) override
        {
            return visitor.visit(*this);
        }
    };

    struct Unary : public Expression
    {
        Unary() = default;
        Unary(Token operatorType, std::unique_ptr<Expression> right)
            :
            operatorType (operatorType), right(std::move(right)){};

        Token operatorType;
        std::unique_ptr<Expression> right;

        ExprVisitorString::RetType accept(ExprVisitorString& visitor) override
        {
            return visitor.visit(*this);
        }
    };
}