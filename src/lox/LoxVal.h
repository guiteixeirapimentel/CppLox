#pragma once
#include <variant>
#include <memory>
#include <string>
#include "LoxObject.hpp"

namespace pimentel
{
    using LoxVal = std::variant<std::shared_ptr<LoxObject>, void*, double, std::string, bool>;
}