#pragma once
#include "ExpressionVisitor.hpp"
#include <memory>
#include <variant>
#include "Token.h"

namespace pimentel
{
    class LoxObject;
}

namespace pimentel
{

    class Interpreter : public ExprVisitorLoxVal
    {
    public:
        ~Interpreter() = default;

        void interpret(Expression& expr);

        RetType visit(Binary&) override;
        RetType visit(Grouping&) override;
        RetType visit(Literal&) override;
        RetType visit(Unary&) override;

    private:
        RetType evaluate(Expression&);

    };
}
