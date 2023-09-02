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
    class BlockStmt;
    class IfStmt;
    class WhileStmt;
    class BreakStmt;
    class ForStmt;
    class FunctionDeclStmt;
    class ReturnStmt;
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
        virtual RetType visit(BlockStmt&) = 0;
        virtual RetType visit(IfStmt&) = 0;
        virtual RetType visit(WhileStmt&) = 0;
        virtual RetType visit(BreakStmt&) = 0;
        virtual RetType visit(ForStmt&) = 0;
        virtual RetType visit(FunctionDeclStmt&) = 0;
        virtual RetType visit(ReturnStmt&) = 0;
    };

    using StmtVisitor = StmtVisitor_T<void>;


} // namespace pimentel
