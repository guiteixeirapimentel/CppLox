#pragma once
#include <chrono>
#include "Expression.h"
#include "ExpressionVisitor.hpp"

namespace pimentel
{
    class ClockFnc : public LoxCallable
    {
    public:
        ClockFnc(){  }
        ~ClockFnc() = default;

        LoxVal call(Interpreter&, const std::vector<LoxVal>&, Environment&) override
        {
            const auto now = std::chrono::system_clock::now();
            const auto duration = now.time_since_epoch();
            return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
                    .count()/1000.0;
        }

        size_t arity() const override
        {
            return 0;
        }
    };

}
