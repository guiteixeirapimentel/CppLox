#pragma once
#include "ExpressionVisitor.hpp"
#include "StmtVisitor.hpp"
#include <memory>
#include <vector>
#include <variant>
#include "Token.h"

namespace pimentel
{
    class LoxObject;
}

namespace pimentel
{

    class Interpreter : public ExprVisitorLoxVal, public StmtVisitor
    {
    public:
        using RetType_expr = ExprVisitorLoxVal::RetType;
        using RetType_stmt = StmtVisitor::RetType;

    public:
        ~Interpreter() = default;

        void interpret(const std::vector<std::unique_ptr<Statement>>& stmts);

    private:

        void execute(Statement& stmt);

        RetType_expr visit(Binary&) override;
        RetType_expr visit(Grouping&) override;
        RetType_expr visit(Literal&) override;
        RetType_expr visit(Unary&) override;

        RetType_stmt visit(ExpressionStmt&) override;
        RetType_stmt visit(PrintStmt&) override;

        RetType_expr evaluate(Expression&);

    };
}
