cmake_minimum_required(VERSION 3.16)
project(web-server)

set(CMAKE_CXX_STANDARD 98)

#if(STANDART98)
#    set(CMAKE_CXX_STANDARD 98)
#else()
#    set(CMAKE_CXX_STANDARD 11)
#endif()


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
        srcs/http/HeaderValue.cpp
        srcs/utilities/SharedPtr.h srcs/core/events/ParseHttpRequestEvent.cpp srcs/core/events/ParseHttpRequestEvent.h)

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
