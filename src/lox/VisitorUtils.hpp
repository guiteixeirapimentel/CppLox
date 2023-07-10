#pragma once

// I really would preffer not using macros, but as I can't use
// templated virtual functions, this is the best solution to reduce code
// duplication.
#define ACCEPT_IMPL(type) \
type::RetType accept(type& visitor) override \
{ \
    return visitor.visit(*this);\
}
