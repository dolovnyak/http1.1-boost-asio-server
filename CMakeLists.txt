cmake_minimum_required(VERSION 3.16)
project(dtl_test_webserver)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_executable(${PROJECT_NAME}
        main.cpp)

#add_custom_target(CHANGE_SOMAXCONN ALL
#        echo 1024 > /proc/sys/net/core/somaxconn
#        VERBATIM
#        )
#
#add_dependencies(${PROJECT_NAME} CHANGE_SOMAXCONN)