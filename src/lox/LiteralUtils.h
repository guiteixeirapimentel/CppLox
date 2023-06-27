#pragma once
#include <string>
#include "Token.h"
#include "CustomTraits.h"

namespace pimentel
{
    std::string literalToString(Token::LiteralType literal);
}