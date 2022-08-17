%{
#include "Grammar.yy.hpp"
#include <iostream>
%}

%option nounput
%option noinput

%%

[0-9]+                  {yylval.IntNumber = std::stoi(yytext); return WS_INTEGER_NUMBER;}
[a-zA-Z\-_]+            {yylval.word = strdup(yytext); return WS_HEADER_KEY;}
[a-zA-Z0-9_\-\.\/]+     {yylval.word = strdup(yytext); return WS_VALUE_PART;}
" :"                    {return WS_COLON;} /* it should be with space */
"\r\n"                  {return WS_CRLF;}
"\r"                    {return WS_CR;}
"\n"                    {return WS_LF;}
[\t\v\f ]+              {;}

.                       {throw std::runtime_error("LEX EXCEPTION: lexeme \"" + std::string(yytext) + "\" doesn't correct");}

%%

int yywrap (void)
{
	return 1;
}