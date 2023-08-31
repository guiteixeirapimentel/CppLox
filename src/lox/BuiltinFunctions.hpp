#pragma once
#include <chrono>
#include "Expression.h"
#include "ExpressionVisitor.hpp"

namespace pimentel
{
    class ClockFnc : public LoxCallable
    {
    public:
        ClockFnc(){ arity = 0; }
        ~ClockFnc() = default;

        LoxVal call(ExpressionVisitor<LoxVal>&, const std::vector<LoxVal>&) override
        {
            const auto now = std::chrono::system_clock::now();
            const auto duration = now.time_since_epoch();
            return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
                    .count()/1000.0;
        }
    };

}
