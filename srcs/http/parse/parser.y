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
%token FIRST_LINE                           "first-line"
%token HEADER                               "header"

%token END 0                                "end-of-input"

%token SINGLE_SPACE                         "single-space"

%token <std::string> ABSOLUTE_PATH          "absolute-path"
%token <std::string> TCHAR_STRING           "tchar-strign"
%token <std::string> QUERY_STRING           "query-string"
%token <std::string> VCHAR_STRING           "vchar-string"
%token <std::string> SPACES_STRING          "spaces-string"

%token <std::pair<int, int> > HTTP_VERSION  "http-version"
%token COLON                                "colon"

%type <std::string> header_key              "header-key"
%type <std::string> header_value            "header-value"

%right SPACES_STRING

%locations

%code requires
{
    #include "HttpException.h"
    #include "utilities.h"
    #include "Lexer.h"

    #include <iostream>
    #include <string>
    #include <utility>

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
                    | HEADER header

/// https://www.rfc-editor.org/rfc/rfc7230#section-3.1.1
first_line:         http_method SINGLE_SPACE request-target SINGLE_SPACE http_version

/// https://www.rfc-editor.org/rfc/rfc7230#section-3.1.1
http_method:        TCHAR_STRING {
                        request.method = $1;
                        lexer.SetNextExpectedTokenGroup(yy::Token::Resource);
                    }

/// only origin form supported https://www.rfc-editor.org/rfc/rfc7230#section-5.3.1
request-target:     ABSOLUTE_PATH {
                        lexer.SetNextExpectedTokenGroup(yy::Token::HttpVersion);
                        request.target.path = $1;
                    }
                    | ABSOLUTE_PATH QUERY_STRING {
                        lexer.SetNextExpectedTokenGroup(yy::Token::HttpVersion);
                        request.target.path = $1;
                        request.target.query = $2;
                    }

/// https://www.rfc-editor.org/rfc/rfc7230#section-2.6
http_version:       HTTP_VERSION {
                        request.http_version.major = $1.first;
                        request.http_version.minor = $1.second;
                    }


/// https://www.rfc-editor.org/rfc/rfc7230#section-3.2
header:             header_key optional_spaces header_value optional_spaces {
                        request.AddHeader($1, $3);
                    }

optional_spaces:    SPACES_STRING
                    | %empty

header_key:         TCHAR_STRING COLON {
                        lexer.SetNextExpectedTokenGroup(yy::Token::HeaderValue);
                        $$ = $1;
                    }

header_value:       VCHAR_STRING {
                        $$ = $1;
                    }
                    | header_value SPACES_STRING VCHAR_STRING {
                        $$ = $1 + $2 + $3;
                    }

%%

void yy::Parser::error(const location &loc , const std::string &message) {
    std::cout << "Error: " << message << std::endl << "Location: " << loc << std::endl;
}
