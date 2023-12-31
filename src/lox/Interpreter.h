#pragma once
#include "ExpressionVisitor.hpp"
#include "StmtVisitor.hpp"
#include <memory>
#include <vector>
#include <variant>
#include "Token.h"
#include "Environment.h"

#include <sstream>

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
        Interpreter(std::ostream& printStream);
        Interpreter();
        ~Interpreter() = default;

        void interpret(const std::vector<std::unique_ptr<Statement>>& stmts);

    private:

        void execute(Statement& stmt);

        RetType_expr visit(Binary&) override;
        RetType_expr visit(Grouping&) override;
        RetType_expr visit(Literal&) override;
        RetType_expr visit(Unary&) override;
        RetType_expr visit(Variable&) override;
        RetType_expr visit(Assignment&) override;
        RetType_expr visit(Logical&) override;
        RetType_expr visit(Call&) override;
        RetType_expr visit(Indexing&) override;

        RetType_stmt visit(ExpressionStmt&) override;
        RetType_stmt visit(PrintStmt&) override;
        RetType_stmt visit(VarStmt&) override;
        RetType_stmt visit(BlockStmt&) override;
        RetType_stmt visit(IfStmt&) override;
        RetType_stmt visit(WhileStmt&) override;
        RetType_stmt visit(BreakStmt&) override;
        RetType_stmt visit(ForStmt&) override;
        RetType_stmt visit(FunctionDeclStmt&) override;
        RetType_stmt visit(ReturnStmt&) override;

        RetType_expr evaluate(Expression&);
    public:
        void executeBlock(const std::vector<std::unique_ptr<Statement>>& stmts, const std::shared_ptr<Environment>& env);

    private:
        std::shared_ptr<Environment> m_env;
        std::shared_ptr<Environment> m_currEnv;

        std::ostream& m_printStream;

        bool m_foundBreakStmt;
    };
}
