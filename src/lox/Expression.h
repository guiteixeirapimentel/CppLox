#pragma once
#include "Token.h"

#include "ExpressionVisitor.hpp"

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
        Binary(Expression* left, Token operatorType, Expression* right)
            :
            left(left), operatorType(operatorType), right(right){}

        Expression* left;
        Token operatorType;
        Expression* right;

        ExprVisitorString::RetType accept(ExprVisitorString& visitor) override
        {
            return visitor.visit(*this);
        }
    };

    struct Grouping : public Expression
    {
        Grouping() = default;
        Grouping(Expression* expr) : expr(expr){};
        Expression* expr;

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
        Unary(Token operatorType, Expression* right)
            :
            operatorType (operatorType),right(right){};

        Token operatorType;
        Expression* right;

        ExprVisitorString::RetType accept(ExprVisitorString& visitor) override
        {
            return visitor.visit(*this);
        }
    };
}