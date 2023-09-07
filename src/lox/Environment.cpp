#include "Environment.h"
#include "ErrorManager.h"

using namespace pimentel;

Environment::Environment(const std::shared_ptr<Environment>& enclosing)
    :
    m_enclosing(enclosing)
{}

Environment::Environment()
    :
    Environment(nullptr)
{}

void Environment::define(const std::string& name, LoxVal value)
{
    m_vars.emplace(name, value);
}

void Environment::assign(const std::string& name, LoxVal value)
{
    auto it = m_vars.find(name);

    if(it != m_vars.end())
    {
        it->second = value;
        return;
    }

    if(m_enclosing)
    {
        m_enclosing->assign(name, value);
        return;
    }

    ErrorManager::get().report(0, "Undefined variable '" + name + "'.");
}

LoxVal Environment::get(const std::string& name) const
{
    const auto it = m_vars.find(name);
    
    if(it != m_vars.end())
    {
        return it->second;
    }

    if(m_enclosing)
    {
        return m_enclosing->get(name);
    }

    ErrorManager::get().report(0, "Variable does not exist: " + name);
    return {};
}
