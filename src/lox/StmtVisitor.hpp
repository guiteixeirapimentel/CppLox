#pragma once
#include <variant>
#include <string>
#include <memory>
namespace pimentel
{
    class Statement;
    class ExpressionStmt;
    class PrintStmt;
    class VarStmt;
}

namespace pimentel
{
    template<typename T>
    class StmtVisitor_T
    {
    public:
        using RetType = T;
    public:
        virtual ~StmtVisitor_T() = default;

        virtual RetType visit(ExpressionStmt&) = 0;
        virtual RetType visit(PrintStmt&) = 0;
        virtual RetType visit(VarStmt&) = 0;
    };

    using StmtVisitor = StmtVisitor_T<void>;


} // namespace pimentel
