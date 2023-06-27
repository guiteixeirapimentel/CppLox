#pragma once
#include "ExpressionVisitor.hpp"

#include "Expression.h"
#include <array>

#include <string>
#include <sstream>

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
                        std::array{expr.left, expr.right});
        }

        RetType visit(Grouping& expr) override 
        {
            return parenthesize("group",
                std::array{expr.expr});
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
                std::array{expr.right});
        }

    private:
        template<typename T>
        std::string parenthesize(const std::string& name, const T& exprPtrList)
        {
            std::stringstream stream;

            stream << "(" << name;
            for(const auto& expr : exprPtrList)
            {
                stream << " ";
                stream << expr->accept(*this);
            }

            stream <<  ")";

            return stream.str();
        }
    };
}
