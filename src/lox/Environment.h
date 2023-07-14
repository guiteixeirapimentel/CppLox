#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Token.h"
#include "LoxVal.h"

namespace pimentel
{
    class Environment
    {
    public:
        Environment();
        Environment(Environment* enclosing);
        ~Environment() = default;

        void define(const std::string& name, LoxVal value);
        void assign(const std::string& name, LoxVal value);
        LoxVal get(const std::string& name) const;

    private:
        Environment* m_enclosing;
        std::unordered_map<std::string, LoxVal> m_vars;
    };
}