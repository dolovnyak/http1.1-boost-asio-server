cmake_minimum_required(VERSION 3.16)
project(web-server)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pthread -O2 -D POLL")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")


add_executable(${PROJECT_NAME}
        srcs/main.cpp
        srcs/WebServer.h
        srcs/config/Config.cpp
        srcs/cgi/SingleCgiHandler.cpp
        srcs/core/Poll.cpp
        srcs/core/Epoll.cpp
        srcs/core/Event.h srcs/core/ServerInstance.h)

target_include_directories(${PROJECT_NAME} PUBLIC
        srcs
        srcs/config
        srcs/utilities
        srcs/cgi
        srcs/webserver
        srcs/core
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
        )

target_compile_definitions(${PROJECT_NAME} PRIVATE -D_DEBUG)
