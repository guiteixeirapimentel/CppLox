
set(LOX_SOURCE 
    Lox.cpp
    Lox.h
    Scanner.cpp
    Scanner.h
    Token.cpp
    Token.h
    ErrorManager.h
    ErrorManager.cpp
    LiteralUtils.h
    LiteralUtils.cpp
    Expression.h
    ExpressionVisitor.hpp
    AstPrinter.hpp
    Parser.cpp
    Parser.h
    Interpreter.cpp
    Interpreter.h
    Environment.cpp
    Environment.h
    LoxVal.h
    UserFunction.h
    UserFunction.cpp
)

add_library(lox_lib ${LOX_SOURCE})

add_library(lox::Lox ALIAS lox_lib)