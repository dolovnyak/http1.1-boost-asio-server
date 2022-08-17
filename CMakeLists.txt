cmake_minimum_required(VERSION 3.16)
project(web-server)

set(STANDART98 1)

if (STANDART98)
    set(CMAKE_CXX_STANDARD 98)
else ()
    set(CMAKE_CXX_STANDARD 11)
endif ()


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_executable(${PROJECT_NAME}
        srcs/main.cpp
        srcs/config/Config.cpp
        srcs/cgi/SingleCgiHandler.cpp
        srcs/core/modules/PollModule.cpp
        srcs/core/events/ParseHttpRequestEvent.cpp
        srcs/http/HttpErrorCodePages.cpp
        srcs/http/Request.cpp
        srcs/http/Response.cpp
        srcs/http/HeaderValue/BaseHeaderValue.cpp
        srcs/utilities/utilities.cpp
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
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
        )

target_compile_definitions(${PROJECT_NAME} PRIVATE -D_DEBUG -D_STANDART98)

################################################################################
# tests
################################################################################
if (!STANDART98)
    message(aaaa)
    set(WS_TESTS ${PROJECT_NAME}-tests)
    add_subdirectory(googletest)

    add_executable(${WS_TESTS}
            tests/test_utilities.cpp
            )

    target_include_directories(${WS_TESTS} PUBLIC
            googletest/googletest/include
            )
endif ()