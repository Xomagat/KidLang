cmake_minimum_required(VERSION 3.31.11)
project(kid)

set(CMAKE_CXX_STANDARD 20)

add_executable(kid main.cpp
        Parser/Lexer.cpp
        Parser/Lexer.h
        Parser/Token.cpp
        Parser/Token.h
        Parser/TokenType.h
        Parser/AST/Expression.h
        Parser/Parser.cpp
        Parser/Parser.h
        Parser/AST/Statement.h
        Parser/AST/BinExpression.h
        libs/Value.h
        libs/NumberValue.h
        libs/BoolValue.h
        libs/StringValue.h
        Parser/AST/ValueExpression.h
        Parser/AST/UnaryExpression.h
        Parser/AST/IOStatement.h
        Parser/AST/AssignmentStatement.h
        libs/Environment.h
        Parser/AST/VariablesExpression.h
        Parser/AST/IfStatement.h
        Parser/AST/ConditionalExpression.h
        Parser/AST/LoopStatement.h)

if (WIN32)
    target_link_options(kid PRIVATE -static-libgcc -static-libstdc++)
elseif (UNIX)
    target_link_options(kid PRIVATE -static)
endif()