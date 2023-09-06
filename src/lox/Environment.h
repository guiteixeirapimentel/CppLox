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
        Environment(const Environment&) = delete;
        Environment(Environment&&) = delete;
        Environment(Environment* enclosing);
        ~Environment() = default;

        Environment& operator=(const Environment&) = delete;
        Environment& operator=(Environment&&) = delete;

        void define(const std::string& name, LoxVal value);
        void assign(const std::string& name, LoxVal value);
        LoxVal get(const std::string& name) const;

        bool returnFlagSet() const
        {
            return m_returnFlag;
        }

        void setReturnFlag(bool flag)
        {
            m_returnFlag = flag;
        }

        LoxVal getReturnValue() const
        {
            return m_returnVal;
        }

        void setReturnVal(LoxVal val)
        {
            m_returnVal = val;
        }

    private:
        Environment* m_enclosing;
        std::unordered_map<std::string, LoxVal> m_vars;

        bool m_returnFlag = false;
        LoxVal m_returnVal = {};
    };
}