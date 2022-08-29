#pragma once

#include "Lexer.h"
#include "Parser.h"
#include <sstream>

class Request;

class ParserWrapper {
public:
    ParserWrapper(Request& request) : _lexer(), _parser(_lexer, request) {
        _lexer.switch_streams(&_ss, nullptr);
    }

    void Parse(const std::string& raw, size_t start, size_t end, yy::ParseState state) {
        _lexer.set_parse_state(state);
        _ss.write(const_cast<char*>(raw.c_str()) + start, end - start);
        _parser.parse();
    }

private:
    yy::Lexer _lexer;
    yy::Parser _parser;
    std::stringstream _ss;
};

