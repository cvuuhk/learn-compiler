cmake_minimum_required(VERSION 3.30)
project(json-parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

add_executable(json-parser)
target_sources(json-parser PRIVATE "src/main.cpp")
target_include_directories(json-parser PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# spdlog
add_subdirectory(deps/spdlog)
target_link_libraries(json-parser PRIVATE spdlog::spdlog)
