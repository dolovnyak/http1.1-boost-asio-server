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
%token FIRST_LINE "first-line"
%token EXPRESSION "expression"

%locations

%code requires
{
    #include <iostream>
    #include <string>
    #include "Lexer.h"

    namespace yy {
	class Lexer;
    }
}

%code top
{
    #include "Lexer.h"
    #include "Parser.h"
    #include "location.hh"

    static yy::Parser::symbol_type yylex(yy::Lexer& lexer) {
        return lexer.get_next_token();
    }

    using namespace yy;
}

%start meta_start

%%

meta_start:	FIRST_LINE first_line
		| EXPRESSION expression

first_line:	STRING STRING STRING { std::cout << "first line" << std::endl; }

expression:	STRING STRING STRING { std::cout << "expression" << std::endl; }

%%

void yy::Parser::error(const location &loc , const std::string &message) {
    std::cout << "Error: " << message << std::endl << "Location: " << loc << std::endl;
}
