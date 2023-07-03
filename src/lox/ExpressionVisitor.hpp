#pragma once
#include <variant>
#include <string>
#include <memory>
#include "Token.h"
#include "LoxObject.hpp"

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
    };

    using ExprVisitorString = ExpressionVisitor<std::string>;

    using LoxVal = std::variant<std::unique_ptr<LoxObject>, void*, double, std::string, bool>;

    using ExprVisitorLoxVal = ExpressionVisitor<LoxVal>;

} // namespace pimentel
