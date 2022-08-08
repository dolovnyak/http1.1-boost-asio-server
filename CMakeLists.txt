cmake_minimum_required(VERSION 3.16)
project(dtl_test_webserver)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pthread -O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_executable(${PROJECT_NAME}
        srcs/main.cpp
        srcs/web_server/WebServer.cpp
        srcs/config/Config.cpp
        srcs/cgi/SingleCgiHandler.cpp
        srcs/utilities/log.h)

target_include_directories(${PROJECT_NAME} PUBLIC
        srcs
        srcs/config
        srcs/web_server
        srcs/cgi
        )
