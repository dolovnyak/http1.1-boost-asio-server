cmake_minimum_required(VERSION 3.16)
project(web-server)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pthread -D POLL")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread -D POLL")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")


add_executable(${PROJECT_NAME}
        srcs/main.cpp
        srcs/config/Config.cpp
        srcs/cgi/SingleCgiHandler.cpp
        srcs/core/modules/PollModule.cpp
        srcs/core/modules/EpollModule.cpp
        srcs/core/events/Event.cpp
        srcs/http/HttpModule.cpp
        srcs/core/events/EventPresets.cpp
        srcs/http/Request.cpp
        srcs/http/Response.cpp
        srcs/http/HttpErrorCodePages.h
        srcs/libjtoc/src/jtoc_get.cpp
        srcs/libjtoc/src/jtoc_get_raw_data.cpp
        srcs/libjtoc/src/jtoc_node.cpp
        srcs/libjtoc/src/jtoc_parse.cpp
        srcs/libjtoc/src/jtoc_parse_number.cpp
        srcs/libjtoc/src/jtoc_parse_value.cpp
        srcs/libjtoc/src/jtoc_read.cpp
        srcs/libjtoc/src/jtoc_read_file.cpp
        srcs/libjtoc/src/jtoc_utils.cpp
        srcs/libjtoc/src/jtoc_utils2.cpp
        srcs/libjtoc/src/jtoc_validate.cpp
        srcs/libjtoc/src/jtoc_validate_value.cpp
        srcs/libjtoc/src/jtoc_write.cpp
        srcs/libjtoc/src/ft_atoi.cpp
        srcs/libjtoc/src/ft_bzero.cpp
        srcs/libjtoc/src/ft_isdigit.cpp
        srcs/libjtoc/src/ft_itoa.cpp
        srcs/libjtoc/src/ft_memalloc.cpp
        srcs/libjtoc/src/ft_memcpy.cpp
        srcs/libjtoc/src/ft_strcpy.cpp
        srcs/libjtoc/src/ft_strdup.cpp
        srcs/libjtoc/src/ft_strhash.cpp
        srcs/libjtoc/src/ft_strlen.cpp
        srcs/libjtoc/src/ft_strncmp.cpp
        srcs/libjtoc/src/ft_strnew.cpp
        srcs/libjtoc/src/ft_strsub.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        srcs
        srcs/config
        srcs/utilities
        srcs/cgi
        srcs/webserver
        srcs/core
        srcs/core/events
        srcs/core/modules
        srcs/http
        srcs/libjtoc/include
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
        )

target_compile_definitions(${PROJECT_NAME} PRIVATE -D_DEBUG)
