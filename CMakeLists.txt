cmake_minimum_required(VERSION 3.16)
project(web-server)

#set(STANDARD98 1)

if (STANDARD98)
    set(CMAKE_CXX_STANDARD 98)
else ()
    set(CMAKE_CXX_STANDARD 11)
endif ()


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

################################################################################
# web-server-lib target
################################################################################
set(WS_LIB ${PROJECT_NAME}-lib)

add_library(${WS_LIB}
        srcs/config/Config.cpp
        srcs/cgi/SingleCgiHandler.cpp
        srcs/core/modules/PollModule.cpp
        srcs/http/Request.cpp
        srcs/http/Response.cpp
        srcs/http/parse/parse.cpp
        srcs/http/errors/HttpErrorPages.cpp
        srcs/http/errors/HttpException.cpp
        srcs/utilities/utilities.cpp
        )

target_include_directories(${WS_LIB} PUBLIC
        srcs
        srcs/config
        srcs/utilities
        srcs/cgi
        srcs/webserver
        srcs/core
        srcs/core/events
        srcs/core/modules
        srcs/http
        srcs/http/parse
        srcs/http/errors
        )

if (STANDARD98)
    target_compile_definitions(${WS_LIB} PRIVATE -D_STANDARD98)
else ()
    target_compile_definitions(${WS_LIB} PRIVATE -D_DEBUG)
endif ()

################################################################################
# web-server target
################################################################################
add_executable(${PROJECT_NAME}
        srcs/main.cpp
        )
target_link_libraries(${PROJECT_NAME} PUBLIC ${WS_LIB})

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
        )


################################################################################
# tests
################################################################################
if (NOT STANDARD98)
    set(WS_TESTS ${PROJECT_NAME}-tests)
    add_subdirectory(googletest)

    add_executable(${WS_TESTS}
            tests/test_utilities.cpp
            tests/test_handle_request.cpp
            )

    target_link_libraries(${WS_TESTS}
            gtest
            gtest_main
            ${WS_LIB})

    target_include_directories(${WS_TESTS} PUBLIC
            googletest/googletest/include
            )
endif ()