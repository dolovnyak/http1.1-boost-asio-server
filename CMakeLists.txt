cmake_minimum_required(VERSION 3.16)
project(dtl_test_webserver)

set(CMAKE_CXX_STANDARD 98)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pthread -O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_executable(${PROJECT_NAME}
        srcs/old_main.cpp srcs/SingleCgiHandler.cpp srcs/SingleCgiHandler.h srcs/main.cpp srcs/Config.cpp srcs/Config.h)
