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

#include "Parser.h"


namespace yy {

    enum ParseState {
        None = 0,
        FirstLine,
        Expression
    };

    class Lexer : public yyFlexLexer {
    public:
        Lexer() : _parse_state(yy::None) {}

        virtual ~Lexer() {}

        virtual yy::Parser::symbol_type get_next_token();

        void set_parse_state(yy::ParseState state) {
            _parse_state = state;
        }

    private:
        ParseState _parse_state;
    };

}
