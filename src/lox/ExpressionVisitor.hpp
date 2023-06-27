#pragma once

namespace pimentel
{
    class Binary;
    class Grouping;
    class Literal;
    class Unary;
}

namespace pimentel
{
    template<typename T>
    class ExpressionVisitor
    {
    public:
        using RetType = T;
    public:
        ExpressionVisitor() = default;
        virtual ~ExpressionVisitor() = default;

        virtual RetType visit(Binary&) = 0;
        virtual RetType visit(Grouping&) = 0;
        virtual RetType visit(Literal&) = 0;
        virtual RetType visit(Unary&) = 0;
    };

    using ExprVisitorString = ExpressionVisitor<std::string>;

} // namespace pimentel
