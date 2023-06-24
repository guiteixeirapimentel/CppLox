
set(LOX_SOURCE 
    Lox.cpp
    Lox.h
    Scanner.cpp
    Scanner.h
    Token.cpp
    Token.h
    ErrorManager.h
    ErrorManager.cpp
)

add_library(lox_lib ${LOX_SOURCE})

add_library(lox::Lox ALIAS lox_lib)