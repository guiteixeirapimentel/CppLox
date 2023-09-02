#pragma once
#include <variant>
#include <memory>
#include <string>
#include <vector>
#include "LoxObject.hpp"

namespace pimentel
{
    class LoxCallable;
    class Environment;

    struct LoxCallableStub
    {
        LoxCallableStub() = default;
        virtual ~LoxCallableStub() = default;
        virtual LoxCallable& get() = 0;
    };

    class Interpreter;
}

namespace pimentel
{
    using LoxVal = std::variant<std::shared_ptr<LoxObject>, void*, double, std::string, bool, std::shared_ptr<LoxCallableStub>>;

    class LoxCallable : public LoxCallableStub
    {
    public:
        LoxCallable& get() override { return *this; }
        virtual LoxVal call(Interpreter& interpreter, const std::vector<LoxVal>& argList, Environment& curEnv) = 0;

        virtual size_t arity() const = 0;
    };
}