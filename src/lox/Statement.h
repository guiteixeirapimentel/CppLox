#pragma once
#include <memory>
#include "Expression.h"
#include "StmtVisitor.hpp"

namespace pimentel
{
    struct Statement
    {
        Statement() = default;
        virtual ~Statement() = default;

        virtual StmtVisitor::RetType accept(StmtVisitor& visitor) = 0;
    };

    struct ExpressionStmt : public Statement
    {
        ExpressionStmt() = default;
        ExpressionStmt(std::unique_ptr<Expression> expr): expr(std::move(expr)){};

        ACCEPT_IMPL(StmtVisitor);

        std::unique_ptr<Expression> expr;
    };

    struct PrintStmt : public Statement
    {
        PrintStmt() = default;
        PrintStmt(std::unique_ptr<Expression> expr): expr(std::move(expr)){};

        ACCEPT_IMPL(StmtVisitor);

        std::unique_ptr<Expression> expr;
    };

    struct VarStmt : public Statement
    {
        VarStmt() = default;
        VarStmt(const Token& name)
            :
            VarStmt(name, {nullptr})
        {}
        VarStmt(const Token& name, std::unique_ptr<Expression> init)
            :
            initializer(std::move(init)),
            name(name)
        {}

        ACCEPT_IMPL(StmtVisitor);

        std::unique_ptr<Expression> initializer = nullptr;
        Token name;
    };

    struct BlockStmt : public Statement
    {
        BlockStmt() = default;
        BlockStmt(std::vector<std::unique_ptr<Statement>>&& stmts)
            :
            stmts(std::move(stmts))
        {}
        ~BlockStmt() = default;

        ACCEPT_IMPL(StmtVisitor);

        std::vector<std::unique_ptr<Statement>> stmts;
    };

    struct IfStmt : public Statement
    {
        IfStmt() = default;
        IfStmt(std::unique_ptr<Expression> expr, std::unique_ptr<Statement> block, std::unique_ptr<Statement> elseblock)
            :
            expr(std::move(expr)),
            block(std::move(block)),
            elseblock(std::move(elseblock))
        {}
        ~IfStmt() = default;

        ACCEPT_IMPL(StmtVisitor);

        std::unique_ptr<Expression> expr;
        std::unique_ptr<Statement> block;
        std::unique_ptr<Statement> elseblock;
    };

    struct WhileStmt : public Statement
    {
        WhileStmt() = default;

        WhileStmt(std::unique_ptr<Expression> expr, std::unique_ptr<Statement> block)
            :
            expr(std::move(expr)),
            block(std::move(block))
        {}

        ~WhileStmt() = default;

        ACCEPT_IMPL(StmtVisitor);

        std::unique_ptr<Expression> expr;
        std::unique_ptr<Statement> block;
    };
}