#include "UserFunction.h"
#include "Interpreter.h"

using namespace pimentel;

UserFunction::UserFunction(std::unique_ptr<BlockStmt>&& block, std::vector<std::string>&& argNames, const std::shared_ptr<Environment>& curEnv)
        :
        m_block(std::move(block)),
        m_argNames(std::move(argNames)),
        m_currEnv(curEnv)
    {}

LoxVal UserFunction::call(Interpreter& interpreter, const std::vector<LoxVal>& argList)
{
    auto fEnv = std::make_shared<Environment>(m_currEnv);

    for(size_t i = 0; i < argList.size(); i++)
    {
        fEnv->define(m_argNames[i], argList[i]);
    }

    interpreter.executeBlock(m_block->stmts, fEnv);

    if(fEnv->returnFlagSet())
    {
        m_currEnv->setReturnFlag(false);
        m_currEnv->setReturnVal({});
        return fEnv->getReturnValue();
    }

    return {};
}

size_t UserFunction::arity() const
{
    return m_argNames.size();
}