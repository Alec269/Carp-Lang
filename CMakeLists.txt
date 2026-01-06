cmake_minimum_required(VERSION 3.30)

## 1️⃣ Enable experimental import std FIRST
#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD          #keep commented for visual studio
#    d0edc3af-4c50-42ea-a356-e2862fe7a444
#)

# 2️⃣ Now declare the project (this detects the compiler)
project(CarpLang LANGUAGES CXX)

# 3️⃣ C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 4️⃣ Enable import std (NOW it exists)
#set(CMAKE_CXX_MODULE_STD ON)            #keep commented for visual studio
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add executable
add_executable(${PROJECT_NAME})


target_sources(${PROJECT_NAME} PUBLIC
    src/main.cpp
    src/tokeniser.cpp
    src/headers/tokeniser.hpp
)

# C++ Modules MUST go in a FILE_SET
# target_sources(${PROJECT_NAME} PRIVATE
#     FILE_SET CXX_MODULES FILES
#     src/tokeniser.ixx
# )

# # Enable import std
# set_target_properties(${PROJECT_NAME} PROPERTIES
#     CXX_MODULE_STD ON
# )

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

set(ABS_BIN_DIR ${CMAKE_SOURCE_DIR}/out/build/bin)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${ABS_BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${ABS_BIN_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${ABS_BIN_DIR}
)

# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug-only flags
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

if(NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-g -O0>
    )
endif()

# Enable testing
enable_testing()

# Install target
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
