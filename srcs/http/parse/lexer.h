#pragma once

#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer wsFlexLexer // the trick with prefix; no namespace here :(

#include <FlexLexer.h>

#endif

// Scanner method signature is defined by this macro. Original yylex() returns int.
// Sinice Bison 3 uses symbol_type, we must change returned type. We also rename it
// to something sane, since you cannot overload return type.
#undef YY_DECL
#define YY_DECL yy::Parser::symbol_type yy::Lexer::get_next_token()

#include "parser.hpp" // this is needed for symbol_type

namespace yy {

class Lexer : public yyFlexLexer {
public:
    virtual ~Lexer() {}

    virtual yy::Parser::symbol_type get_next_token();
};

}
