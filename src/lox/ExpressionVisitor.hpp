#pragma once
#include "LoxVal.h"

namespace pimentel
{
    class LoxObject;
}

namespace pimentel
{
    class Expression;
    class Binary;
    class Grouping;
    class Literal;
    class Unary;
    class Variable;
    class Assignment;
    class Logical;
    class Call;
    class Indexing;
}

namespace pimentel
{
    template<typename T>
    class ExpressionVisitor
    {
    public:
        using RetType = T;
    public:
        virtual ~ExpressionVisitor() = default;

        virtual RetType visit(Binary&) = 0;
        virtual RetType visit(Grouping&) = 0;
        virtual RetType visit(Literal&) = 0;
        virtual RetType visit(Unary&) = 0;
        virtual RetType visit(Variable&) = 0;
        virtual RetType visit(Assignment&) = 0;
        virtual RetType visit(Logical&) = 0;
        virtual RetType visit(Call&) = 0;
        virtual RetType visit(Indexing&) = 0;
    };

    using ExprVisitorString = ExpressionVisitor<std::string>;

    using ExprVisitorLoxVal = ExpressionVisitor<LoxVal>;

} // namespace pimentel
