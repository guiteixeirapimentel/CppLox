#include "LiteralUtils.h"


std::string pimentel::literalToString(Token::LiteralType literal)
{
    return std::visit(overloaded{
        [](const std::string& arg) { return arg; },
        [](void* unused_) { (void)unused_; return std::string{"NULL"}; },
        [](const bool& arg) { return std::string{arg ? "true" : "false"}; },
        [](const auto& arg) { return std::to_string(arg); },
    }, literal);
}