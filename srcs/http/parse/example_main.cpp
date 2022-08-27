#include "Lexer.h"
#include "Parser.h"
#include <sstream>
#define _DEBUG
#include "../../utilities/Logging.h"

int main() {
    yy::location loc;

    LOG_START_TIMER("MAIN");
    std::string str = "123 123 123";
    std::stringstream ss;
    ss << str;

    yy::Lexer lexer;
    lexer.set_parse_state(yy::ParseState::Expression);
    lexer.switch_streams(&ss, nullptr);
    yy::Parser parser(lexer);
    parser.parse();
    std::cout << "end first" << std::endl;

    lexer.set_parse_state(yy::ParseState::FirstLine);
    ss << "abc abc abc";
    parser.parse();

    LOG_TIME();
}
