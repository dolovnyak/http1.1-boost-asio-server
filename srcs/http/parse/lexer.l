%option c++
%option noyywrap
%option outfile="Lexer.cpp"
%option yyclass="Lexer"
%option prefix="ws"

%{
#include "location.hh"
#include "Lexer.h"
#include "Parser.h"
#include "utilities.h"

#include <iostream>
#include <utility>

#define yyterminate() yy::Parser::make_END(yy::location());

%}

%s HTTP_METHOD
%s RESOURCE
%s HTTP_VERSION
%s HEADER_KEY
%s HEADER_VALUE

/* from https://www.rfc-editor.org/rfc/rfc7230#section-1.2 */
vchar_string        [\x20-\x7E]+

/* from https://www.rfc-editor.org/rfc/rfc7230#section-3.2.6 http_token using this */
tchar_string        [a-zA-Z0-9!#$%&'*+.^_`|~\-]+

/* from https://www.rfc-editor.org/rfc/rfc3986#section-2.3 */
unreserved_string   [a-zA-Z0-9\-._~]+

/* from https://www.rfc-editor.org/rfc/rfc3986#section-2.2 */
sub_delims_string   [!$&'()*+,;=]+

/* from https://www.rfc-editor.org/rfc/rfc3986#section-2.1 */
pct_encoded_string  %([0-9a-fA-F]{2})+

/* from https://www.rfc-editor.org/rfc/rfc3986#section-3.3 */
pchar_string        ({unreserved_string}|{pct_encoded_string}|{sub_delims_string})+


/* from https://www.rfc-editor.org/rfc/rfc3986#section-3.3 */
absolute_path       ("/"{pchar_string}?)+

/* from https://www.rfc-editor.org/rfc/rfc3986#section-3.4 */
query_string        ({pchar_string}|"/"|"?")+

%%

%{
switch (_parse_state) {
    case None:
        break;

    case FirstLine:
        //std::cout << "first line" << std::endl;
        _parse_state = None;
        return yy::Parser::make_FIRST_LINE(yy::location());
    case Header:
        //std::cout << "header" << std::endl;
        _parse_state = None;
        return yy::Parser::make_HEADER(yy::location());
}
%}


<HTTP_METHOD>{tchar_string} { std::cout << "Lexer http method" << std::endl; return yy::Parser::make_TCHAR_STRING(yytext, yy::location()); }

<RESOURCE>"?"{query_string}? { std::cout << "Lexer query string" << std::endl; return yy::Parser::make_QUERY_STRING(yytext + 1, yy::location()); }

<RESOURCE>{absolute_path} { std::cout << "Lexer absolute path" << std::endl; return yy::Parser::make_ABSOLUTE_PATH(yytext, yy::location()); }

<HTTP_VERSION>"HTTP/"[0-9].[0-9] {
                    std::cout << "lexer HTTP_VERSION: " << yytext << std::endl;
                    int major = GetIntFromDigit(*(yytext + 5));
                    int minor = GetIntFromDigit(*(yytext + 7));
                    return yy::Parser::make_HTTP_VERSION(std::make_pair(major, minor), yy::location());
                    }

<HEADER_KEY>{tchar_string} { std::cout << "Lexer header key" << std::endl; return yy::Parser::make_TCHAR_STRING(yytext, yy::location()); }
<HEADER_KEY>":"             { std::cout << "Lexer header key colon" << std::endl; return yy::Parser::make_COLON(yy::location()); }

<HEADER_VALUE>{vchar_string} { std::cout << "Lexer header value" << std::string(yytext) << std::endl;
                                return yy::Parser::make_VCHAR_STRING(StripString(yytext), yy::location()); }

[ ]             { return yy::Parser::make_SINGLE_SPACE(yy::location()); }

.               { std::cout << "Scanner: unknown character [" << yytext << "]" << std::endl; }

<<EOF>>         { std::cout << "EOF" << std::endl;
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
            case yy::Token::HeaderKey:
                std::cout << "Lexer: header key pushed" << std::endl;
                yy_push_state(HEADER_KEY);
                break;
            case yy::Token::HeaderValue:
                std::cout << "Lexer: header value pushed" << std::endl;
                yy_push_state(HEADER_VALUE);
                break;
            default:
                throw std::runtime_error("unsupported token type in SetNextExpectedTokenGroup");
        }
    }

