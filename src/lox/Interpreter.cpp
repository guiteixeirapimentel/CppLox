#include "Interpreter.h"
#include "Expression.h"
#include "Statement.h"
#include "CustomTraits.h"
#include "LiteralUtils.h"
#include "ErrorManager.h"
#include <cassert>
#include <iostream>

using namespace pimentel;

namespace
{
    bool isTruthy(const Interpreter::RetType_expr& literal)
    {
        return std::visit(overloaded{
            [](const bool& val) { return val; },
            [](const double& val) { return val != 0.0; },
            [](void* val) { return val != nullptr; },
            [](const std::shared_ptr<LoxObject>& obj) { return obj.get() != nullptr; },
            [](const auto&) { return true; }
        }, literal);
    }

    template<typename T1>
    Interpreter::RetType_expr treat(const Interpreter::RetType_expr& val1, const Interpreter::RetType_expr& val2,
        const T1& literalCallback, const auto& mismatchingCallback)
    {
        if(val1.index() != val2.index())
        {
            return mismatchingCallback(val1, val2);
        }
        return literalCallback(val1, val2);
        
        /*assert(0);*/

        return Interpreter::RetType_expr{};
    }

    Interpreter::RetType_expr handleMismatching(const auto& val1, const auto& val2, const auto& token)
    {
        if(token.getType() == TokenType::EQUAL_EQUAL)
        {
            return Interpreter::RetType_expr{false};
        }

        (void)val1;
        (void)val2;

        ErrorManager::get().report(token, "Mismatch types - Could not find overloaded operator.");
        return Interpreter::RetType_expr{};
    }
}

Interpreter::RetType_expr Interpreter::visit(Binary& expr)
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
                    return RetType_expr{leftStr == rightStr};
                case TokenType::BANG_EQUAL:
                    return RetType_expr{leftStr != rightStr};
                break;
                case TokenType::PLUS:
                    return RetType_expr{leftStr + rightStr};
                break;
                default:
                    /*assert(0);*/
                    return RetType_expr{};
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
                    return RetType_expr{leftV != rightV};
                break;
                case TokenType::EQUAL_EQUAL:
                    return RetType_expr{leftV == rightV};
                default:
                    /*assert(0);*/
                    return RetType_expr{};
                break;
            }
            return RetType_expr{};
        };

        const auto handleNumeric = [&rightVal, &expr](const double& leftV)
        {
            using T = std::remove_cvref_t<decltype(leftV)>;
            const auto rightV = *std::get_if<T>(&rightVal);

            switch(expr.operatorType.getType())
            {
                case TokenType::MINUS:
                    return RetType_expr{leftV - rightV};
                break;
                case TokenType::PLUS:
                    return RetType_expr{leftV + rightV};
                break;
                case TokenType::SLASH:
                    return RetType_expr{leftV / rightV};
                break;
                case TokenType::STAR:
                    return RetType_expr{leftV * rightV};
                break;
                case TokenType::GREATER:
                    return RetType_expr{leftV > rightV};
                break;
                case TokenType::GREATER_EQUAL:
                    return RetType_expr{leftV >= rightV};
                break;
                case TokenType::LESS:
                    return RetType_expr{leftV < rightV};
                break;
                case TokenType::LESS_EQUAL:
                    return RetType_expr{leftV <= rightV};
                break;
                case TokenType::BANG_EQUAL:
                    return RetType_expr{leftV != rightV};
                break;
                case TokenType::EQUAL_EQUAL:
                    return RetType_expr{leftV == rightV};
                break;
                default:
                    /*assert(0);*/
                    return RetType_expr{};
                break;
            }
        };

        return std::visit(overloaded{
            [](void*){ return RetType_expr{}; },
            [](const std::shared_ptr<LoxObject>&){ return RetType_expr{}; },
            handleString,
            handleBoolean,
            handleNumeric,
        }, leftVal);
    },
    [&expr](const auto& lhs, const auto& rhs) {
        return handleMismatching(lhs, rhs, expr.operatorType);
    });
}

Interpreter::RetType_expr Interpreter::visit(Grouping& expr)
{
    return evaluate(*expr.expr);
}

Interpreter::RetType_expr Interpreter::visit(Literal& expr)
{
    return std::visit([](const auto& val)
        { return RetType_expr{val};}, expr.value);
}

Interpreter::RetType_expr Interpreter::visit(Unary& expr)
{
    auto rhs = evaluate(*expr.right);

    switch(expr.operatorType.getType())
    {
        case TokenType::MINUS:
            return std::visit(overloaded{
                [](const double& rhs) { return RetType_expr{-rhs}; },
                [](const auto& rhs) { (void) rhs; /*assert(0);*/ return RetType_expr{};}
            }, rhs);
        break;
        case TokenType::BANG:
            return RetType_expr{!isTruthy(rhs)};
        break;
        default:
            /*assert(0);*/
            return RetType_expr{};
        break;
    }
}

Interpreter::RetType_expr Interpreter::visit(Variable& var)
{
    return m_currEnv->get(var.name.getLexeme());
}

Interpreter::RetType_expr Interpreter::visit(Assignment& expr)
{
    auto value = evaluate(*expr.value);

    m_currEnv->assign(expr.name.getLexeme(), value);

    return value;
}

Interpreter::RetType_expr Interpreter::visit(Logical& logical)
{
    auto val = isTruthy(evaluate(*logical.leftExpr));
    switch(logical.op.getType())
    {
    case TokenType::AND:
        if(val)
        {
            val = val && isTruthy(evaluate(*logical.rightExpr));
        }
    break;
    case TokenType::OR:
        val = val || isTruthy(evaluate(*logical.rightExpr));
    break;
    default:
        ErrorManager::get().report(logical.op, "Invalid logical type!");
    break;
    }

    return RetType_expr{val};
}

Interpreter::RetType_stmt Interpreter::visit(ExpressionStmt& exprStmt)
{
    evaluate(*exprStmt.expr);
}

Interpreter::RetType_stmt Interpreter::visit(PrintStmt& printStmt)
{
    auto val = evaluate(*printStmt.expr);

    std::visit(overloaded{
        [this](const std::shared_ptr<LoxObject>& obj) {
            m_printStream << "[Lox obj] = " << obj.get() << std::endl;
        },
        [this](const std::string& arg) { m_printStream << arg << std::endl; },
        [this](void* unused_) { (void)unused_; m_printStream <<  std::string{"NULL"} << std::endl; },
        [this](const bool& arg) { m_printStream << std::string{arg ? "true" : "false"} << std::endl; },
        [this](const auto& arg) { m_printStream << std::to_string(arg) << std::endl; },
    }, val);
}

Interpreter::RetType_stmt pimentel::Interpreter::visit(VarStmt& varStmt)
{
    m_currEnv->define(varStmt.name.getLexeme(),
        varStmt.initializer ? evaluate(*varStmt.initializer) : LoxVal{});
}

Interpreter::RetType_stmt Interpreter::visit(BlockStmt& blockStmt)
{
    auto env = Environment{m_currEnv};
    executeBlock(blockStmt.stmts, env);
}

Interpreter::RetType_stmt Interpreter::visit(IfStmt& ifStmt)
{
    auto exprVal = evaluate(*ifStmt.expr);

    if(isTruthy(exprVal))
    {
        ifStmt.block->accept(*this);
    }
    else if(ifStmt.elseblock)
    {
        ifStmt.elseblock->accept(*this);
    }
}

Interpreter::RetType_stmt Interpreter::visit(WhileStmt& whileStmt)
{
    while(isTruthy(evaluate(*whileStmt.expr)) && !m_foundBreakStmt)
    {
        whileStmt.block->accept(*this);
    }

    m_foundBreakStmt = false;
}

Interpreter::RetType_stmt Interpreter::visit(BreakStmt&)
{
    m_foundBreakStmt = true;
}

Interpreter::RetType_stmt pimentel::Interpreter::visit(ForStmt& forStmt)
{
    auto env = Environment{m_currEnv};
    Environment* previous = m_currEnv;

    m_currEnv = &env;

    forStmt.variableDef->accept(*this);
    while(isTruthy(evaluate(*forStmt.expr)) && !m_foundBreakStmt)
    {
        forStmt.block->accept(*this);
        forStmt.incStmt->accept(*this);
    }

    m_foundBreakStmt = false;

    m_currEnv = previous;
}

Interpreter::RetType_expr Interpreter::evaluate(Expression& expr)
{
    return expr.accept(*this);
}

void pimentel::Interpreter::executeBlock(const std::vector<std::unique_ptr<Statement>>& stmts, Environment& env)
{
    Environment* previous = m_currEnv;

    m_currEnv = &env;

    for(const auto& stmt : stmts)
    {
        execute(*stmt);
        if(m_foundBreakStmt)
        {
            break;
        }
    }

    m_currEnv = previous;
}

void Interpreter::execute(Statement& stmt)
{
    stmt.accept(*this);
}

Interpreter::Interpreter(std::ostream& printStream)
    :
    m_env(),
    m_currEnv(&m_env),
    m_printStream(printStream),
    m_foundBreakStmt(false)
{}

Interpreter::Interpreter()
    :
    Interpreter(std::cout)
{}

void Interpreter::interpret(const std::vector<std::unique_ptr<Statement>>& stmts)
{
    for (const auto& stmt : stmts)
    {
        execute(*stmt);
    }
}
