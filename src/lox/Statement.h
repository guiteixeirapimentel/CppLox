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
}