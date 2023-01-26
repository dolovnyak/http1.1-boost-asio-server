cmake_minimum_required(VERSION 3.16)
project(web-server)

set(CMAKE_CXX_STANDARD 20)


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

################################################################################
# lib-mini-event target
################################################################################
set(LIB_MINI_EVENT lib-mini-event)
add_library(${LIB_MINI_EVENT}
        lib_mini_event/events/DefaultCloseEvent.cpp
)

target_include_directories(${LIB_MINI_EVENT} PUBLIC
        lib_mini_event/
        lib_mini_event/events
        lib_mini_event/sessions
        lib_mini_event/modules
        )

################################################################################
# web-server-lib target
################################################################################
set(WS_LIB ${PROJECT_NAME}-lib)
add_library(${WS_LIB}
        srcs/config/Config.cpp
        lib_mini_event/modules/poll_module/PollModule.cpp
        srcs/core/sessions/HttpSession.cpp
        srcs/core/core_helpers.cpp
        srcs/http/Request.cpp
        srcs/http/Response.cpp
        srcs/http/errors/HttpErrorPages.cpp
        srcs/http/errors/HttpException.cpp
        srcs/http/Http.cpp
        srcs/utilities/utilities.cpp
        srcs/utilities/Logging.cpp
        )

add_subdirectory(libjtoc)

target_include_directories(${WS_LIB} PUBLIC
        srcs
        srcs/config
        srcs/utilities
        srcs/cgi
        srcs/webserver
        srcs/core
        srcs/core/events
        srcs/core/modules
        srcs/core/sessions
        srcs/http
        srcs/http/errors
        srcs/http/resource_handlers
        libjtoc/include
        )

target_link_libraries(${WS_LIB} PUBLIC
        ${LIB_MINI_EVENT}
        jtoc
        )

if (STANDARD98)
    target_compile_definitions(${WS_LIB} PUBLIC -D_STANDARD98)
else ()
    target_compile_definitions(${WS_LIB} PUBLIC -D_DEBUG)
endif ()

################################################################################
# web-server target
################################################################################
add_executable(${PROJECT_NAME}
        srcs/main.cpp
        srcs/config/parse_config.cpp srcs/config/parse_config.h)
target_link_libraries(${PROJECT_NAME} PUBLIC ${WS_LIB})

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
        )


################################################################################
# tests
################################################################################
# if (NOT STANDARD98)
#     set(WS_TESTS ${PROJECT_NAME}-tests)
#     add_subdirectory(googletest)

#     add_executable(${WS_TESTS}
#             tests/test_utilities.cpp
#             tests/test_fsm_handle_request.cpp
#             tests/test_poll_module.cpp
#             )

#     target_link_libraries(${WS_TESTS}
#             gtest
#             gtest_main
#             ${WS_LIB})

#     target_include_directories(${WS_TESTS} PUBLIC
#             googletest/googletest/include
#             )
# endif ()