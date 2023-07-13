#pragma once
#include <unordered_map>
#include <string>
#include "Token.h"
#include "LoxVal.h"

namespace pimentel
{
    class Environment
    {
    public:
        Environment() = default;
        ~Environment() = default;

        void define(const std::string& name, LoxVal value);
        void assign(const std::string& name, LoxVal value);
        LoxVal get(const std::string& name) const;

    private:
        std::unordered_map<std::string, LoxVal> m_globalVars;
    };
}