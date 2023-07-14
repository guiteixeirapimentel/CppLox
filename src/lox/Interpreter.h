#pragma once
#include "ExpressionVisitor.hpp"
#include "StmtVisitor.hpp"
#include <memory>
#include <vector>
#include <variant>
#include "Token.h"
#include "Environment.h"

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

        void interpret(const std::vector<std::unique_ptr<Statement>>& stmts);

    private:

        void execute(Statement& stmt);

        RetType_expr visit(Binary&) override;
        RetType_expr visit(Grouping&) override;
        RetType_expr visit(Literal&) override;
        RetType_expr visit(Unary&) override;
        RetType_expr visit(Variable&) override;
        RetType_expr visit(Assignment&) override;

        RetType_stmt visit(ExpressionStmt&) override;
        RetType_stmt visit(PrintStmt&) override;
        RetType_stmt visit(VarStmt&) override;
        RetType_stmt visit(BlockStmt&) override;

        RetType_expr evaluate(Expression&);

        void executeBlock(const std::vector<std::unique_ptr<Statement>>& stmts, Environment& env);

    private:
        Environment m_env;
        Environment* m_currEnv = &m_env;
    };
}
