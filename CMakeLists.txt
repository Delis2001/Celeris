cmake_minimum_required(VERSION 3.16)

project(Celeris LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Celeris
    src/main.cpp

    src/ast/ast.cpp
    src/ast/ast_manager.cpp

    src/core/APINode.cpp
    src/core/databaseNode.cpp
    src/core/FileSystem.cpp
    src/core/RatelimitNode.cpp
    src/core/RouteNode.cpp
    src/core/ServerNode.cpp

    src/db/databaseCon.cpp

    src/net/cleansocket.cpp
    src/net/clientsock.cpp
    src/net/serversock.cpp
    src/net/socket.cpp
    src/net/Socket_manager.cpp

    src/protocols/http/httprequest.cpp

    src/util/fileParser.cpp
    src/util/mimetypes.cpp
    src/util/PathResolver.cpp
)

target_include_directories(Celeris PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Link Winsock on Windows
if(WIN32)
    target_link_libraries(Celeris PRIVATE ws2_32)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(Celeris PRIVATE /W4)
else()
    target_compile_options(Celeris PRIVATE -Wall -Wextra -Wpedantic)
endif()