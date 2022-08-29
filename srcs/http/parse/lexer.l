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

%s HTTP_METHOD
%s RESOURCE
%S HTTP_VERSION

tchar_string        [a-zA-Z0-9!#$%&'*+.^_`|~\-]+
unreserved_string   [a-zA-Z0-9\-._~]+
sub_delims_string   [!$&'()*+,;=]+
pct_encoded_string  %([0-9a-fA-F]{2})+
pchar_string        unreserved_string | pct_encoded_string | sub_delims_string
absolute_path       "/"pchar_string*

%%

%{
switch (_parse_state) {
    case None:
        break;

    case FirstLine:
        //std::cout << "first line" << std::endl;
        _parse_state = None;
        return yy::Parser::make_FIRST_LINE(yy::location());
}
%}


<HTTP_METHOD>{tchar_string}     { return yy::Parser::make_TCHAR_STRING(yytext, yy::location()); }

<RESOURCE>{pchar_string}   { return yy::Parser::make_ABSOLUTE_PATH(yytext, yy::location()); }

<HTTP_VERSION>"HTTP/"[0-9].[0-9] {
                    std::cout << "lexer HTTP_VERSION: " << yytext << std::endl;
                    return yy::Parser::make_HTTP_VERSION(yytext, yy::location());
                    }

[ ]             { return yy::Parser::make_SINGLE_SPACE(yy::location()); }

.               { std::cout << "Scanner: unknown character [" << yytext << "]" << std::endl; }

<<EOF>>         { //std::cout << "EOF" << std::endl;
                    return yyterminate();
                }

%%

    void yy::Lexer::SetNextExpectedTokenGroup(yy::Token::TokenType token_type) {
        switch (token_type) {
            case yy::Token::Default:
                yy_push_state(INITIAL);
                break;
            case yy::Token::HttpMethod:
                yy_push_state(HTTP_METHOD);
                break;
            case yy::Token::Resource:
                yy_push_state(RESOURCE);
                break;
            case yy::Token::HttpVersion:
                yy_push_state(HTTP_VERSION);
                break;
            default:
                throw std::runtime_error("unsupported token type in SetNextExpectedTokenGroup");
        }
    }
