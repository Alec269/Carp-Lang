cmake_minimum_required(VERSION 3.30)

## Enable experimental import std FIRST
#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD          #keep commented for visual studio
#    d0edc3af-4c50-42ea-a356-e2862fe7a444
#)

# Now declare the project (this detects the compiler)
project(CarpLang VERSION 0.0.15 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 4️⃣ Enable import std (NOW it exists)
#set(CMAKE_CXX_MODULE_STD ON)            #keep commented for visual studio
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===== LLVM minimal build config =====
set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "" FORCE)

set(LLVM_ENABLE_TERMINFO OFF CACHE BOOL "" FORCE)
set(LLVM_ENABLE_ZLIB OFF CACHE BOOL "" FORCE)
set(LLVM_ENABLE_ZSTD OFF CACHE BOOL "" FORCE)

set(LLVM_INCLUDE_TESTS OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_BENCHMARKS OFF CACHE BOOL "" FORCE)

set(LLVM_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(LLVM_BUILD_UTILS OFF CACHE BOOL "" FORCE)

# set(LLVM_ENABLE_PROJECTS "lld" CACHE STRING "" FORCE)



# add_subdirectory(external/llvm-21.1.8/llvm EXCLUDE_FROM_ALL)

# llvm_map_components_to_libnames(LLVM_LIBS
#     core
#     support
#     irreader
#     target
#     x86
# )

# Add executable
add_executable(${PROJECT_NAME})

# target_link_libraries(${PROJECT_NAME} PRIVATE ${LLVM_LIBS})


target_sources(${PROJECT_NAME} PUBLIC
    src/main.cpp
    src/tokeniser.cpp
    src/parser.cpp
    src/headers/tokeniser.hpp
    src/headers/parser.hpp
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

# target_include_directories(${PROJECT_NAME} PRIVATE
#     ${CMAKE_SOURCE_DIR}/include
# )

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
