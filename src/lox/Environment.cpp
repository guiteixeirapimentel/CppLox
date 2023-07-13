#include "Environment.h"
#include "ErrorManager.h"

using namespace pimentel;

void Environment::define(const std::string& name, LoxVal value)
{
    m_globalVars.emplace(name, value);
}

void Environment::assign(const std::string& name, LoxVal value)
{
    auto it = m_globalVars.find(name);

    if(it == m_globalVars.end())
    {
        ErrorManager::get().report(0, "Undefined variable '" + name + "'.");
        return;
    }

    it->second = value;
}

LoxVal Environment::get(const std::string& name) const
{
    const auto it = m_globalVars.find(name);

    if(it == m_globalVars.end())
    {
        ErrorManager::get().report(0, "Variable does not exist: " + name);
        return {};
    }

    return it->second;
}
