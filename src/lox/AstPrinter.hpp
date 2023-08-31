#pragma once
#include "ExpressionVisitor.hpp"

#include "Expression.h"
#include <array>

#include <string>
#include <sstream>
#include <functional>

#include "LiteralUtils.h"

namespace pimentel
{
    class AstPrinter : public ExprVisitorString
    {
    public:
        AstPrinter() = default;
        ~AstPrinter() = default;

        std::string print(Expression& expr)
        {
            return expr.accept(*this);
        }

        RetType visit(Binary& expr) override
        {
            return parenthesize(expr.operatorType.getLexeme(),
                        std::array{
                            std::reference_wrapper{*expr.left},
                            std::reference_wrapper{*expr.right}
                        });
        }

        RetType visit(Grouping& expr) override 
        {
            return parenthesize("group",
                std::array{std::reference_wrapper{*expr.expr}});
        }

        RetType visit(Literal& expr) override
        {
            if(std::holds_alternative<void*>(expr.value))
            {
                return "NIL";
            }
            return literalToString(expr.value);
        }
    
        RetType visit(Unary& expr) override
        {
            return parenthesize(expr.operatorType.getLexeme(),
                std::array{std::reference_wrapper{*expr.right}});
        }

        RetType visit(Variable& expr) override
        {
            return expr.name.getLexeme();
        }

        RetType visit(Assignment& expr) override
        {
            return expr.name.getLexeme();
        }

        RetType visit(Logical& expr) override
        {
            return expr.op.getLexeme();
        }

        RetType visit(Call& expr) override
        {
            return expr.paren.getLexeme();
        }

    private:
        template<typename T>
        std::string parenthesize(const std::string& name, const T& exprRefWrapList)
        {
            std::stringstream stream;

            stream << "(" << name;
            for(const auto& expr : exprRefWrapList)
            {
                stream << " ";
                stream << expr.get().accept(*this);
            }

            stream <<  ")";

            return stream.str();
        }
    };
}
