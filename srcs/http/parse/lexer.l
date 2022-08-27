%option c++
%option noyywrap
%option outfile="Lexer.cpp"
%option yyclass="Lexer"
%option prefix="ws"

%{
#include <iostream>

#include "location.hh"
#include "Lexer.h"
#include "Parser.h"

#define yyterminate() yy::Parser::make_END(yy::location());

%}

%%

%{
switch (_parse_state) {
    case None:
        break;

    case FirstLine:
        std::cout << "first line" << std::endl;
        _parse_state = None;
        return yy::Parser::make_FIRST_LINE(yy::location());

    case Expression:
        std::cout << "expression" << std::endl;
        _parse_state = None;
        return yy::Parser::make_EXPRESSION(yy::location());
}
%}

[a-zA-Z0-9]+    {
                    std::cout << "Scanner: identifier [" << yytext << "]" << std::endl;
                    return yy::Parser::make_STRING(yytext, yy::location());
                }

[\n\t ]         {
                    std::cout << "Scanner: whitechar (ignored)" << std::endl;
                }

.               { std::cout << "Scanner: unknown character [" << yytext << "]" << std::endl; }

<<EOF>>         { std::cout << "EOF" << std::endl; return yyterminate(); }
%%
