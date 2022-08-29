%require "3.0"
%language "C++"

%define api.value.type variant
%define api.token.constructor
%define parse.assert

%define api.parser.class { Parser }

%lex-param      { Lexer& lexer }
%parse-param    { Lexer& lexer }
%parse-param    { Request& request }

%define api.namespace { yy }

///meta tokens:
%token EXPRESSION                   "expression"
%token FIRST_LINE                   "first-line"

%token END 0                        "end-of-input"

%token SINGLE_SPACE                 "single-space"

%token <std::string> ABSOLUTE_PATH  "absolute-path"
%token <std::string> TCHAR_STRING   "tchar-strign"
%token <std::string> HTTP_VERSION   "http-version"

%token <char> SLASH                 "slash"

%locations

%code requires
{
    #include "HttpException.h"
    #include "utilities.h"
    #include "Lexer.h"

    #include <iostream>
    #include <string>

    class Request;

    namespace yy {
        class Lexer;
    }
}

%code top
{
    #include "Lexer.h"
    #include "Parser.h"
    #include "location.hh"
    #include "Request.h"

    static yy::Parser::symbol_type yylex(yy::Lexer& lexer) {
        return lexer.get_next_token();
    }

    using namespace yy;
}

%start meta_start

%%

meta_start:         FIRST_LINE first_line

/// https://www.rfc-editor.org/rfc/rfc7230#section-3.1.1
first_line:         http_method SINGLE_SPACE request-target SINGLE_SPACE http_version

http_method:        TCHAR_STRING {
                        std::cout << "http_method: " << $1 << std::endl;
                        request.method = $1;
                        lexer.SetNextExpectedTokenGroup(yy::Token::Resource);
                    }

/// only origin form supported https://www.rfc-editor.org/rfc/rfc7230#section-5.3.1
request-target:     ABSOLUTE_PATH {
                            std::cout << "request-target: " << $1 << std::endl;
                            lexer.SetNextExpectedTokenGroup(yy::Token::HttpVersion);
                            request.target.path = $1;
                    }

http_version:       HTTP_VERSION {
                        std::cout << "http_version: " << $1 << std::endl;
                        request.http_version = HttpVersion(1, 1);
                    }

%%

void yy::Parser::error(const location &loc , const std::string &message) {
    std::cout << "Error: " << message << std::endl << "Location: " << loc << std::endl;
}
