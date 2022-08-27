set(FLEX_INPUT "Lexer.l")
set(BISON_INPUT "Parser.y")

set(FLEX_CPP "Lexer.cpp")
set(BISON_CPP "Parser.cpp")
set(BISON_H "Parser.h")

set(FLEX_COMMAND  /Users/sbecker/.brew/opt/flex/bin/flex++)
set(BISON_COMMAND /Users/sbecker/.brew/opt/bison/bin/bison)

set(PARSE_DIR "${CMAKE_SOURCE_DIR}/srcs/http/parse")

# generate flex/bison parser when build
add_custom_target(parser_generator ALL
        COMMAND rm -f ${BISON_H} ${FLEX_CPP} ${BISON_CPP}
        COMMAND ${FLEX_COMMAND} ${FLEX_INPUT}
        COMMAND ${BISON_COMMAND} -o ${BISON_CPP} --header=${BISON_H} ${BISON_INPUT}
        COMMENT "generate lex/yacc parser"
        VERBATIM
        )
add_dependencies(${WS_LIB} parser_generator)

# generate lex/yacc parse when reload cmake
execute_process(COMMAND rm -f ${BISON_H} ${FLEX_CPP} ${BISON_CPP}
        WORKING_DIRECTORY ${PARSE_DIR}
        )

execute_process(COMMAND ${FLEX_COMMAND} ${FLEX_INPUT}
        WORKING_DIRECTORY ${PARSE_DIR}
        )

execute_process(COMMAND ${BISON_COMMAND} -o ${BISON_CPP} --header=${BISON_H} ${BISON_INPUT}
        WORKING_DIRECTORY ${PARSE_DIR}
        )
