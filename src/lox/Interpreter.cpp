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
    bool isTruthy(const std::unique_ptr<LoxObject>& obj)
    {
        return obj.get() != nullptr;
    }

    bool isTruthy(const Token::LiteralType& literal)
    {
        return std::visit(overloaded{
            [](const bool& val) { return val; },
            [](void* val) { return val != nullptr; },
            [](const auto&) { return true; }
        }, literal);
    }

    const auto doNothingLiteral = [](const Token::LiteralType&)
        {return Interpreter::RetType{};};
    const auto doNothingLoxObj = [](const std::unique_ptr<LoxObject>&)
        {return Interpreter::RetType{};};

    template<typename T1, typename T2>
    Interpreter::RetType treat(const Interpreter::RetType& val, const T1& literalCallback, const T2& loxObjCallback)
    {
        return std::visit(overloaded{
            [loxObjCallback](const std::unique_ptr<LoxObject>& obj) { return loxObjCallback(obj); },
            [literalCallback](const Token::LiteralType& literal) { return literalCallback(literal); },
            [](const auto& _) { (void)_; /*assert(0);*/ return Interpreter::RetType{}; }
        }, val);
    }

    template<typename T1, typename T2>
    Interpreter::RetType treat(const Interpreter::RetType& val1, const Interpreter::RetType& val2,
        const T1& literalCallback, const T2& loxObjCallback, const auto& mismatchingCallback)
    {
        if(val1.index() != val2.index())
        {
            return mismatchingCallback(val1, val2);
        }

        if(std::holds_alternative<std::unique_ptr<LoxObject>>(val1))
        {
            const auto& ptr1 = *std::get_if<std::unique_ptr<LoxObject>>(&val1);
            const auto& ptr2 = *std::get_if<std::unique_ptr<LoxObject>>(&val2);

            assert(ptr1);
            assert(ptr2);

            return loxObjCallback(ptr1, ptr2);
        }

        if(std::holds_alternative<Token::LiteralType>(val1))
        {
            const auto& lit1 = *std::get_if<Token::LiteralType>(&val1);
            const auto& lit2 = *std::get_if<Token::LiteralType>(&val2);

            return literalCallback(lit1, lit2);
        }
        
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
        [](const Token::LiteralType& literal) {
            std::cout << literalToString(literal) << std::endl;
        }
    }, val);
}

Interpreter::RetType Interpreter::visit(Binary& expr)
{
    auto left = evaluate(*expr.left);
    auto right = evaluate(*expr.right);

    return treat(left, right, [&expr](const auto& leftLit, const auto& rightLit)
    {
        if(leftLit.index() != rightLit.index())
        {
            return handleMismatching(leftLit, rightLit, expr.operatorType);
        }

        return std::visit(overloaded{
            [](void*){ return RetType{}; },
            [&rightLit, &expr](const std::string& leftStr)
            {
                using T = std::remove_cvref_t<decltype(leftStr)>;
                const auto rightStr = *std::get_if<T>(&rightLit);

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
            },
            [&rightLit, &expr](const bool& leftVal)
            {
                using T = std::remove_cvref_t<decltype(leftVal)>;
                const auto rightVal = *std::get_if<T>(&rightLit);

                switch(expr.operatorType.getType())
                {
                    case TokenType::BANG_EQUAL:
                        return RetType{leftVal != rightVal};
                    break;
                    case TokenType::EQUAL_EQUAL:
                        return RetType{leftVal == rightVal};
                    default:
                        /*assert(0);*/
                        return RetType{};
                    break;
                }
                return RetType{};
            },
            [&rightLit, &expr](const double& leftVal)
            {
                using T = std::remove_cvref_t<decltype(leftVal)>;
                const auto rightVal = *std::get_if<T>(&rightLit);

                switch(expr.operatorType.getType())
                {
                    case TokenType::MINUS:
                        return RetType{leftVal - rightVal};
                    break;
                    case TokenType::PLUS:
                        return RetType{leftVal + rightVal};
                    break;
                    case TokenType::SLASH:
                        return RetType{leftVal / rightVal};
                    break;
                    case TokenType::STAR:
                        return RetType{leftVal * rightVal};
                    break;
                    case TokenType::GREATER:
                        return RetType{leftVal > rightVal};
                    break;
                    case TokenType::GREATER_EQUAL:
                        return RetType{leftVal >= rightVal};
                    break;
                    case TokenType::LESS:
                        return RetType{leftVal < rightVal};
                    break;
                    case TokenType::LESS_EQUAL:
                        return RetType{leftVal <= rightVal};
                    break;
                    case TokenType::BANG_EQUAL:
                        return RetType{leftVal != rightVal};
                    break;
                    case TokenType::EQUAL_EQUAL:
                        return RetType{leftVal == rightVal};
                    break;
                    default:
                        /*assert(0);*/
                        return RetType{};
                    break;
                }
            }
        }, leftLit);
    }, [](const auto&, const auto&) { return RetType{};},
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
    return expr.value;
}

Interpreter::RetType Interpreter::visit(Unary& expr)
{
    auto rhs = evaluate(*expr.right);

    return treat(rhs,
        [&](const auto& lit){
            switch(expr.operatorType.getType())
            {
                case TokenType::MINUS:
                    return std::visit(overloaded{
                        [](const double& rhs) { return RetType{-rhs}; },
                        [](const auto& rhs) { (void) rhs; /*assert(0);*/ return RetType{};}
                    }, rhs);
                break;
                case TokenType::BANG:
                    return RetType{!isTruthy(lit)};
                break;
                default:
                    /*assert(0);*/
                    return RetType{};
                break;
            }
        },
        [&](const auto& obj){
            switch(expr.operatorType.getType())
            {
                case TokenType::BANG:
                    return RetType{!isTruthy(obj)};
                break;
                default:
                    /*assert(0);*/
                    return RetType{};
                break;
            }
        }
    );
}

Interpreter::RetType Interpreter::evaluate(Expression& expr)
{
    return expr.accept(*this);
}
