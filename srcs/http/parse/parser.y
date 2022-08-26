%require "3.0"
%language "C++"

%define api.value.type variant
%define api.token.constructor
%define parse.assert

%define api.parser.class { Parser }

%lex-param { Lexer& lexer }
%parse-param { Lexer& lexer }

%define api.namespace { yy }

%token END 0 "end-of-input"
%token <std::string> STRING "string"
%token <int> NUMBER "number"

%locations

%code requires
{
    #include <iostream>
    #include <string>
    #include "lexer.h"

    namespace yy {
	class Lexer;
    }
}

%code top
{
    #include "lexer.h"
    #include "parser.hpp"
    #include "location.hh"

    static yy::Parser::symbol_type yylex(yy::Lexer& lexer) {
        return lexer.get_next_token();
    }

    using namespace yy;
}

%start line

%%

line: STRING { std::cout << "a: " << $1 << std::endl; }

%%

void yy::Parser::error(const location &loc , const std::string &message) {

        // Location should be initialized inside scanner action, but is not in this example.
        // Let's grab location directly from driver class.
	 std::cout << "Error: " << message << std::endl << "Location: " << loc << std::endl;
}
