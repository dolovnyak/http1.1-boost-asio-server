%option c++
%option noyywrap
%option outfile="lexer.cpp"
%option yyclass="Lexer"
%option prefix="ws"

%{
#include <iostream>

#include "location.hh"
#include "Lexer.h"
#include "parser.hpp"

#define yyterminate() yy::Parser::make_END(yy::location());

%}

%%

[a-z]+      {
                std::cout << "Scanner: identifier [" << yytext << "]" << std::endl;
                return yy::Parser::make_STRING(yytext, yy::location());
            }

[\n\t ]     {
                std::cout << "Scanner: whitechar (ignored)" << std::endl;
            }

.           { std::cout << "Scanner: unknown character [" << yytext << "]" << std::endl; }

<<EOF>>     { std::cout << "EOF" << std::endl; return yyterminate(); }
%%
