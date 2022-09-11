cmake_minimum_required(VERSION 3.16)
project(parser)

#set(STANDARD98 1)

if (STANDARD98)
    set(CMAKE_CXX_STANDARD 98)
else ()
    set(CMAKE_CXX_STANDARD 11)
endif ()


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_executable(${PROJECT_NAME}
        Config.cpp
        parse_config.cpp
        parser_test_main.cpp
        ../http/Http.cpp
        ../utilities/utilities.cpp
        ../utilities/Logging.cpp
        ../../libjtoc/src/ft_atoi.cpp
        ../../libjtoc/src/ft_bzero.cpp
        ../../libjtoc/src/ft_isdigit.cpp
        ../../libjtoc/src/ft_itoa.cpp
        ../../libjtoc/src/ft_memalloc.cpp
        ../../libjtoc/src/ft_memcpy.cpp
        ../../libjtoc/src/ft_strcpy.cpp
        ../../libjtoc/src/ft_strdup.cpp
        ../../libjtoc/src/ft_strhash.cpp
        ../../libjtoc/src/ft_strlen.cpp
        ../../libjtoc/src/ft_strncmp.cpp
        ../../libjtoc/src/ft_strnew.cpp
        ../../libjtoc/src/ft_strsub.cpp
        ../../libjtoc/src/jtoc_get.cpp
        ../../libjtoc/src/jtoc_get_raw_data.cpp
        ../../libjtoc/src/jtoc_node.cpp
        ../../libjtoc/src/jtoc_parse.cpp
        ../../libjtoc/src/jtoc_parse_number.cpp
        ../../libjtoc/src/jtoc_parse_value.cpp
        ../../libjtoc/src/jtoc_read.cpp
        ../../libjtoc/src/jtoc_read_file.cpp
        ../../libjtoc/src/jtoc_utils.cpp
        ../../libjtoc/src/jtoc_utils2.cpp
        ../../libjtoc/src/jtoc_validate.cpp
        ../../libjtoc/src/jtoc_validate_value.cpp
        ../../libjtoc/src/jtoc_write.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        ../
        ./
        ../utilities
        # srcs/cgi
        # srcs/webserver
        # srcs/core
        # srcs/core/events
        # srcs/core/modules
        # srcs/core/sessions
        ../http
        # srcs/http/errors
        # srcs/http/resource_handlers
        ../../libjtoc/include
        )

if (STANDARD98)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -D_STANDARD98)
else ()
    target_compile_definitions(${PROJECT_NAME} PUBLIC -D_DEBUG)
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
        )

