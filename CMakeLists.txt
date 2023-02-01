cmake_minimum_required(VERSION 3.16)
project(web-server)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

################################################################################
# web-server-lib target
################################################################################
set(WS_LIB ${PROJECT_NAME}-lib)
add_library(${WS_LIB}
        srcs/config/Config.cpp
        srcs/config/ConfigParser.cpp
        srcs/sessions/HttpSession.cpp
        srcs/core/core_helpers.cpp
        srcs/http/Request.cpp
        srcs/http/Response.cpp
        srcs/http/errors/HttpErrorPages.cpp
        srcs/http/Http.cpp
        srcs/utilities/utilities.cpp
        srcs/utilities/Logging.cpp
        )

target_include_directories(${WS_LIB} PUBLIC
        srcs
        srcs/config
        srcs/utilities
        srcs/cgi
        srcs/webserver
        srcs/core
        srcs/events
        srcs/core/modules
        srcs/sessions
        srcs/http
        srcs/http/errors
        srcs/http/resource_handlers
        libjtoc/include
        )

target_link_libraries(${WS_LIB} PUBLIC
        ${LIB_MINI_EVENT}
        jtoc
        )

# Boost asio
find_package(Boost REQUIRED COMPONENTS system thread regex REQUIRED)
target_include_directories(${WS_LIB} PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(${WS_LIB} LINK_PUBLIC ${Boost_LIBRARIES})

target_compile_definitions(${WS_LIB} PUBLIC -D_DEBUG)

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
set(WS_TESTS ${PROJECT_NAME}-tests)
add_subdirectory(googletest)

add_executable(${WS_TESTS}
        tests/test_utilities.cpp
        tests/test_fsm_handle_request.cpp
        )

target_link_libraries(${WS_TESTS}
        gtest
        gtest_main
        ${WS_LIB})

target_include_directories(${WS_TESTS} PUBLIC
        googletest/googletest/include
        )
