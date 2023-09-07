#pragma once

#include "LoxVal.h"
#include "Statement.h"
#include "Environment.h"

#include <memory>

namespace pimentel
{

struct UserFunction : public LoxCallable
{
public:
    UserFunction(std::unique_ptr<BlockStmt>&& block, std::vector<std::string>&& argNames, const std::shared_ptr<Environment>& curEnv);
    ~UserFunction() = default;

    LoxVal call(Interpreter& interpreter, const std::vector<LoxVal>& argList) override;

    size_t arity() const override;

private:
    std::unique_ptr<BlockStmt> m_block;
    std::vector<std::string> m_argNames;

    std::shared_ptr<Environment> m_currEnv;
};


}
