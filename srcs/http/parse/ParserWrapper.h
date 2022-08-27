#pragma once

#include "Lexer.h"
#include "Parser.h"
#include <sstream>

class ParserWrapper {
public:
    ParserWrapper() : _lexer(), _parser(_lexer) {
        _lexer.set_debug(true);
    }

    void Parse(const std::string& input, yy::ParseState state) {
        _lexer.set_parse_state(state);
        _ss << input;
        _parser.parse();
    }

private:
    yy::Lexer _lexer;
    yy::Parser _parser;
    std::stringstream _ss;
};

