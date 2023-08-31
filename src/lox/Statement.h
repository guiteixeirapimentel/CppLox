#pragma once
#include <memory>
#include "Expression.h"
#include "StmtVisitor.hpp"

namespace pimentel
{
    enum class ScopeType
    {
        FUNCTION,
        GLOBAL,
        GLOBAL_FUNCTION,
        FOR_WHILE,
        FOR_WHILE_FUNCTION
    };
    struct Statement
    {
        Statement() = default;
        virtual ~Statement() = default;

        virtual StmtVisitor::RetType accept(StmtVisitor& visitor) = 0;
    };

    using StmtPtr = std::unique_ptr<Statement>;

    struct ExpressionStmt : public Statement
    {
        ExpressionStmt() = default;
        ExpressionStmt(ExprPtr expr): expr(std::move(expr)){};

        ACCEPT_IMPL(StmtVisitor);

        ExprPtr expr;
    };

    struct PrintStmt : public Statement
    {
        PrintStmt() = default;
        PrintStmt(ExprPtr expr): expr(std::move(expr)){};

        ACCEPT_IMPL(StmtVisitor);

        ExprPtr expr;
    };

    struct VarStmt : public Statement
    {
        VarStmt() = default;
        VarStmt(const Token& name)
            :
            VarStmt(name, {nullptr})
        {}
        VarStmt(const Token& name, ExprPtr init)
            :
            initializer(std::move(init)),
            name(name)
        {}

        ACCEPT_IMPL(StmtVisitor);

        ExprPtr initializer = nullptr;
        Token name;
    };

    struct BlockStmt : public Statement
    {
        BlockStmt() = default;
        BlockStmt(std::vector<StmtPtr>&& stmts)
            :
            stmts(std::move(stmts))
        {}
        ~BlockStmt() = default;

        ACCEPT_IMPL(StmtVisitor);

        std::vector<StmtPtr> stmts;
    };

    struct IfStmt : public Statement
    {
        IfStmt() = default;
        IfStmt(ExprPtr expr, StmtPtr block, StmtPtr elseblock)
            :
            expr(std::move(expr)),
            block(std::move(block)),
            elseblock(std::move(elseblock))
        {}
        ~IfStmt() = default;

        ACCEPT_IMPL(StmtVisitor);

        ExprPtr expr;
        StmtPtr block;
        StmtPtr elseblock;
    };

    struct WhileStmt : public Statement
    {
        WhileStmt() = default;

        WhileStmt(ExprPtr expr, StmtPtr block)
            :
            expr(std::move(expr)),
            block(std::move(block))
        {}

        ~WhileStmt() = default;

        ACCEPT_IMPL(StmtVisitor);

        ExprPtr expr;
        StmtPtr block;
    };

    struct ForStmt : public Statement
    {
        ForStmt() = default;
        ForStmt(StmtPtr variableDef, ExprPtr expr, ExprPtr incStmt, StmtPtr block)
            :
            variableDef(std::move(variableDef)),
            expr(std::move(expr)),
            incStmt(std::move(incStmt)),
            block(std::move(block))
        {}
        ~ForStmt() = default;

        ACCEPT_IMPL(StmtVisitor);

        StmtPtr variableDef;
        ExprPtr expr;
        ExprPtr incStmt;
        StmtPtr block;
    };

    struct BreakStmt : public Statement
    {
        BreakStmt() = default;
        ~BreakStmt() = default;

        ACCEPT_IMPL(StmtVisitor);
    };
}