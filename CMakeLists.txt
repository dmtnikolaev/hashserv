cmake_minimum_required(VERSION 3.13)

project(hashserv LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=gnu++2a")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)

find_package(Threads REQUIRED)

set(SOURCE src/main.cpp 
        src/transparser.cpp
        src/processing.cpp
        src/pool.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE third_party)
target_link_libraries(${PROJECT_NAME} -lmhash Threads::Threads)
