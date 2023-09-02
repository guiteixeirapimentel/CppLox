#include "Interpreter.h"
#include "Expression.h"
#include "Statement.h"
#include "CustomTraits.h"
#include "LiteralUtils.h"
#include "ErrorManager.h"
#include <cassert>
#include <iostream>

#include "BuiltinFunctions.hpp"

using namespace pimentel;

namespace
{
    void defineBuiltinFunctions(Environment& globalEnv)
    {
        globalEnv.define("clock", std::shared_ptr<LoxCallableStub>(new ClockFnc{}));
    }

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
        if (val1.index() != val2.index())
        {
            return mismatchingCallback(val1, val2);
        }
        return literalCallback(val1, val2);

        /*assert(0);*/

        return Interpreter::RetType_expr{};
    }

    Interpreter::RetType_expr handleMismatching(const auto& val1, const auto& val2, const auto& token)
    {
        if (token.getType() == TokenType::EQUAL_EQUAL)
        {
            return Interpreter::RetType_expr{ false };
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
            if (leftVal.index() != rightVal.index())
            {
                return handleMismatching(leftVal, rightVal, expr.operatorType);
            }

            const auto handleString = [&rightVal, &expr](const std::string& leftStr)
                {
                    using T = std::remove_cvref_t<decltype(leftStr)>;
                    const auto rightStr = *std::get_if<T>(&rightVal);

                    switch (expr.operatorType.getType())
                    {
                    case TokenType::EQUAL_EQUAL:
                        return RetType_expr{ leftStr == rightStr };
                    case TokenType::BANG_EQUAL:
                        return RetType_expr{ leftStr != rightStr };
                        break;
                    case TokenType::PLUS:
                        return RetType_expr{ leftStr + rightStr };
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

                    switch (expr.operatorType.getType())
                    {
                    case TokenType::BANG_EQUAL:
                        return RetType_expr{ leftV != rightV };
                        break;
                    case TokenType::EQUAL_EQUAL:
                        return RetType_expr{ leftV == rightV };
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

                    switch (expr.operatorType.getType())
                    {
                    case TokenType::MINUS:
                        return RetType_expr{ leftV - rightV };
                        break;
                    case TokenType::PLUS:
                        return RetType_expr{ leftV + rightV };
                        break;
                    case TokenType::SLASH:
                        return RetType_expr{ leftV / rightV };
                        break;
                    case TokenType::STAR:
                        return RetType_expr{ leftV * rightV };
                        break;
                    case TokenType::GREATER:
                        return RetType_expr{ leftV > rightV };
                        break;
                    case TokenType::GREATER_EQUAL:
                        return RetType_expr{ leftV >= rightV };
                        break;
                    case TokenType::LESS:
                        return RetType_expr{ leftV < rightV };
                        break;
                    case TokenType::LESS_EQUAL:
                        return RetType_expr{ leftV <= rightV };
                        break;
                    case TokenType::BANG_EQUAL:
                        return RetType_expr{ leftV != rightV };
                        break;
                    case TokenType::EQUAL_EQUAL:
                        return RetType_expr{ leftV == rightV };
                        break;
                    default:
                        /*assert(0);*/
                        return RetType_expr{};
                        break;
                    }
                };

            return std::visit(overloaded{
                [](void*) { return RetType_expr{}; },
                [](const std::shared_ptr<LoxCallableStub>&) { return RetType_expr{}; },
                [](const std::shared_ptr<LoxObject>&) { return RetType_expr{}; },
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
        { return RetType_expr{ val };}, expr.value);
}

Interpreter::RetType_expr Interpreter::visit(Unary& expr)
{
    auto rhs = evaluate(*expr.right);

    switch (expr.operatorType.getType())
    {
    case TokenType::MINUS:
        return std::visit(overloaded{
            [](const double& rhs) { return RetType_expr{-rhs}; },
            [](const auto& rhs) { (void)rhs; /*assert(0);*/ return RetType_expr{};}
            }, rhs);
        break;
    case TokenType::BANG:
        return RetType_expr{ !isTruthy(rhs) };
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
    switch (logical.op.getType())
    {
    case TokenType::AND:
        if (val)
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

    return RetType_expr{ val };
}

Interpreter::RetType_expr Interpreter::visit(Call& callExpr)
{
    auto caleeEvaluated = evaluate(*callExpr.calee);

    std::vector<Interpreter::RetType_expr> args;

    for (const auto& arg : callExpr.arguments)
    {
        args.push_back(evaluate(*arg));
    }

    if (!std::holds_alternative<std::shared_ptr<LoxCallableStub>>(caleeEvaluated))
    {
        ErrorManager::get().report({}, "Trying to call non callable!");
        return {};
    }

    auto& callable = std::get<std::shared_ptr<LoxCallableStub>>(caleeEvaluated)->get();

    if (callable.arity() != args.size())
    {
        std::string err = "Wrong number of args to function: ";
        err += "got " + std::to_string(args.size()) + " expected " + std::to_string(callable.arity());
        ErrorManager::get().report({}, err);
        return {};
    }

    return callable.call(*this, args, *m_currEnv);
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
        [this](void* unused_) { (void)unused_; m_printStream << std::string{"NULL"} << std::endl; },
        [this](const std::shared_ptr<LoxCallableStub>& unused_) { (void)unused_; m_printStream << std::string{"[LoxCallable]"} << std::endl; },
        [this](const bool& arg) { m_printStream << std::string{arg ? "true" : "false"} << std::endl; },
        [this](const auto& arg) { m_printStream << std::to_string(arg) << std::endl; },
        }, val);
}

Interpreter::RetType_stmt pimentel::Interpreter::visit(VarStmt& varStmt)
{
    m_currEnv->define(varStmt.name.getLexeme(),
        varStmt.initializer ? evaluate(*varStmt.initializer) : LoxVal{});
}

Interpreter::RetType_stmt Interpreter::visit(ReturnStmt& retStmt)
{
    m_currEnv->setReturnFlag(true);

    if(retStmt.expr)
    {
        auto resVal = evaluate(*retStmt.expr); // evaluate might set the return flag to false.
        m_currEnv->setReturnFlag(true);
        m_currEnv->setReturnVal(resVal);
    }
}

Interpreter::RetType_stmt Interpreter::visit(BlockStmt& blockStmt)
{
    auto env = Environment{ m_currEnv };
    executeBlock(blockStmt.stmts, env);
}

Interpreter::RetType_stmt Interpreter::visit(IfStmt& ifStmt)
{
    auto exprVal = evaluate(*ifStmt.expr);

    if (isTruthy(exprVal))
    {
        ifStmt.block->accept(*this);
    }
    else if (ifStmt.elseblock)
    {
        ifStmt.elseblock->accept(*this);
    }
}

Interpreter::RetType_stmt Interpreter::visit(WhileStmt& whileStmt)
{
    while (isTruthy(evaluate(*whileStmt.expr)) && !m_foundBreakStmt)
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
    auto env = Environment{ m_currEnv };
    Environment* previous = m_currEnv;

    m_currEnv = &env;

    if (forStmt.variableDef)
    {
        forStmt.variableDef->accept(*this);
    }

    const auto hasExpr = forStmt.expr != nullptr;
    while ((!hasExpr || isTruthy(evaluate(*forStmt.expr))) && !m_foundBreakStmt)
    {
        forStmt.block->accept(*this);
        if (forStmt.incStmt)
        {
            forStmt.incStmt->accept(*this);
        }
    }

    m_foundBreakStmt = false;

    m_currEnv = previous;
}

struct UserFunction : public LoxCallable
{
public:
    UserFunction(std::unique_ptr<BlockStmt>&& block, std::vector<std::string>&& argNames)
        :
        m_block(std::move(block)),
        m_argNames(std::move(argNames))
    {}

    LoxVal call(Interpreter& interpreter, const std::vector<LoxVal>& argList, Environment& curEnv) override
    {
        Environment fEnv{&curEnv};

        for(size_t i = 0; i < argList.size(); i++)
        {
            fEnv.define(m_argNames[i], argList[i]);
        }

        interpreter.executeBlock(m_block->stmts, fEnv);

        if(fEnv.returnFlagSet())
        {
            curEnv.setReturnFlag(false);
            curEnv.setReturnVal({});
            return fEnv.getReturnValue();
        }

        return {};
    }

    size_t arity() const override
    {
        return m_argNames.size();
    }

private:
    std::unique_ptr<BlockStmt> m_block;
    std::vector<std::string> m_argNames;
};

Interpreter::RetType_stmt Interpreter::visit(FunctionDeclStmt& funDecl)
{
    auto funcName = funDecl.name;
    std::vector<std::string> argList;

    std::transform(funDecl.argList.begin(), funDecl.argList.end(),
        std::back_inserter(argList),
        [](const auto& arg) { return arg.getLexeme(); });

    auto uFun = new UserFunction{ std::move(funDecl.block), std::move(argList) };
    m_env.define(funcName.getLexeme(), std::unique_ptr<LoxCallableStub>(uFun));
}

Interpreter::RetType_expr Interpreter::evaluate(Expression& expr)
{
    return expr.accept(*this);
}

void pimentel::Interpreter::executeBlock(const std::vector<StmtPtr>& stmts, Environment& env)
{
    Environment* previous = m_currEnv;

    m_currEnv = &env;

    for (const auto& stmt : stmts)
    {
        execute(*stmt);
        if (m_foundBreakStmt || m_currEnv->returnFlagSet())
        {
            break;
        }
    }

    if(m_currEnv->returnFlagSet())
    {
        previous->setReturnFlag(true);
        previous->setReturnVal(m_currEnv->getReturnValue());
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
{
    defineBuiltinFunctions(m_env);
}

Interpreter::Interpreter()
    :
    Interpreter(std::cout)
{}

void Interpreter::interpret(const std::vector<StmtPtr>& stmts)
{
    for (const auto& stmt : stmts)
    {
        execute(*stmt);
    }
}
