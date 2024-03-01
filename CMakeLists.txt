cmake_minimum_required(VERSION 3.10)
project(ranxin_tinyhttpd VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(src)

add_compile_options(-pthread -g)

link_libraries(-pthread)

# Add executable
add_executable(ranxin_tinyhttpd
    src/main.cpp
    src/server.cpp
    src/connection_handler.cpp
    src/request.cpp
    src/response.cpp
    src/CGI_handler.cpp
    src/static_file_handler.cpp
    src/router.cpp
    src/logger.cpp
)