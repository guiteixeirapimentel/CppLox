#include "Interpreter.h"
#include "Expression.h"
#include "CustomTraits.h"
#include "LiteralUtils.h"
#include "ErrorManager.h"
#include <cassert>
#include <iostream>

using namespace pimentel;

namespace
{
    bool isTruthy(const Interpreter::RetType& literal)
    {
        return std::visit(overloaded{
            [](const bool& val) { return val; },
            [](void* val) { return val != nullptr; },
            [](const std::unique_ptr<LoxObject>& obj) { return obj.get() != nullptr; },
            [](const auto&) { return true; }
        }, literal);
    }

    template<typename T1>
    Interpreter::RetType treat(const Interpreter::RetType& val1, const Interpreter::RetType& val2,
        const T1& literalCallback, const auto& mismatchingCallback)
    {
        if(val1.index() != val2.index())
        {
            return mismatchingCallback(val1, val2);
        }
        return literalCallback(val1, val2);
        
        /*assert(0);*/

        return Interpreter::RetType{};
    }

    Interpreter::RetType handleMismatching(const auto& val1, const auto& val2, const auto& token)
    {
        if(token.getType() == TokenType::EQUAL_EQUAL)
        {
            return Interpreter::RetType{false};
        }

        (void)val1;
        (void)val2;

        ErrorManager::get().report(token, "Mismatch types - Could not find overloaded operator.");
        return Interpreter::RetType{};
    }
}

void Interpreter::interpret(Expression& expr)
{
    auto val = evaluate(expr);

    std::visit(overloaded{
        [](const std::unique_ptr<LoxObject>& obj) {
            std::cout << "[Lox obj] = " << obj.get() << std::endl;
        },
        [](const std::string& arg) { std::cout << arg << std::endl; },
        [](void* unused_) { (void)unused_; std::cout <<  std::string{"NULL"} << std::endl; },
        [](const bool& arg) { std::cout << std::string{arg ? "true" : "false"} << std::endl; },
        [](const auto& arg) { std::cout << std::to_string(arg) << std::endl; },
    }, val);
}

Interpreter::RetType Interpreter::visit(Binary& expr)
{
    auto left = evaluate(*expr.left);
    auto right = evaluate(*expr.right);

    return treat(left, right, [&expr](const auto& leftVal, const auto& rightVal)
    {
        if(leftVal.index() != rightVal.index())
        {
            return handleMismatching(leftVal, rightVal, expr.operatorType);
        }

        const auto handleString = [&rightVal, &expr](const std::string& leftStr)
        {
            using T = std::remove_cvref_t<decltype(leftStr)>;
            const auto rightStr = *std::get_if<T>(&rightVal);

            switch(expr.operatorType.getType())
            {
                case TokenType::EQUAL_EQUAL:
                    return RetType{leftStr == rightStr};
                break;
                case TokenType::PLUS:
                    return RetType{leftStr + rightStr};
                break;
                default:
                    /*assert(0);*/
                    return RetType{};
                break;
            }
        };

        const auto handleBoolean = [&rightVal, &expr](const bool& leftV)
        {
            using T = std::remove_cvref_t<decltype(leftV)>;
            const auto rightV = *std::get_if<T>(&rightVal);

            switch(expr.operatorType.getType())
            {
                case TokenType::BANG_EQUAL:
                    return RetType{leftV != rightV};
                break;
                case TokenType::EQUAL_EQUAL:
                    return RetType{leftV == rightV};
                default:
                    /*assert(0);*/
                    return RetType{};
                break;
            }
            return RetType{};
        };

        const auto handleNumeric = [&rightVal, &expr](const double& leftV)
        {
            using T = std::remove_cvref_t<decltype(leftV)>;
            const auto rightV = *std::get_if<T>(&rightVal);

            switch(expr.operatorType.getType())
            {
                case TokenType::MINUS:
                    return RetType{leftV - rightV};
                break;
                case TokenType::PLUS:
                    return RetType{leftV + rightV};
                break;
                case TokenType::SLASH:
                    return RetType{leftV / rightV};
                break;
                case TokenType::STAR:
                    return RetType{leftV * rightV};
                break;
                case TokenType::GREATER:
                    return RetType{leftV > rightV};
                break;
                case TokenType::GREATER_EQUAL:
                    return RetType{leftV >= rightV};
                break;
                case TokenType::LESS:
                    return RetType{leftV < rightV};
                break;
                case TokenType::LESS_EQUAL:
                    return RetType{leftV <= rightV};
                break;
                case TokenType::BANG_EQUAL:
                    return RetType{leftV != rightV};
                break;
                case TokenType::EQUAL_EQUAL:
                    return RetType{leftV == rightV};
                break;
                default:
                    /*assert(0);*/
                    return RetType{};
                break;
            }
        };

        return std::visit(overloaded{
            [](void*){ return RetType{}; },
            [](const std::unique_ptr<LoxObject>&){ return RetType{}; },
            handleString,
            handleBoolean,
            handleNumeric,
        }, leftVal);
    },
    [&expr](const auto& lhs, const auto& rhs) {
        return handleMismatching(lhs, rhs, expr.operatorType);
    });
}

Interpreter::RetType Interpreter::visit(Grouping& expr)
{
    return evaluate(*expr.expr);
}

Interpreter::RetType Interpreter::visit(Literal& expr)
{
    return std::visit([](const auto& val)
        { return RetType{val};}, expr.value);
}

Interpreter::RetType Interpreter::visit(Unary& expr)
{
    auto rhs = evaluate(*expr.right);

    switch(expr.operatorType.getType())
    {
        case TokenType::MINUS:
            return std::visit(overloaded{
                [](const double& rhs) { return RetType{-rhs}; },
                [](const auto& rhs) { (void) rhs; /*assert(0);*/ return RetType{};}
            }, rhs);
        break;
        case TokenType::BANG:
            return RetType{!isTruthy(rhs)};
        break;
        default:
            /*assert(0);*/
            return RetType{};
        break;
    }
}

Interpreter::RetType Interpreter::evaluate(Expression& expr)
{
    return expr.accept(*this);
}
