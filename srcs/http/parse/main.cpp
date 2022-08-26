#include "lexer.h"
#include "parser.hpp"
#include <sstream>

int main() {
    yy::location loc;

    std::string str = "test";
//    loc.initialize(&str);

    std::stringstream ss(str);

    yy::Lexer lexer;
    lexer.switch_streams(&ss, &std::cout);
//    lexer.yy_switch_to_buffer(lexer.yy_create_buffer(&ss, 4));

    yy::Parser parser(lexer);
    parser.parse();
}
