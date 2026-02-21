cmake_minimum_required(VERSION 3.31)

project("CarpLang" VERSION 0.0.15 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM dir: ${LLVM_DIR}")

llvm_map_components_to_libnames(LLVM_LIBS
    Core
    Support
    IRReader
    X86CodeGen
)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PUBLIC
    src/main.cpp
    src/parser.cpp
    src/SemanticAnalyser.cpp
    src/tokeniser.cpp

    src/headers/parser.hpp
    src/headers/SemanticAnalyser.hpp
    src/headers/tokeniser.hpp
    src/headers/utils.hpp

)

target_include_directories(${PROJECT_NAME}
    SYSTEM PRIVATE
    ${LLVM_INCLUDE_DIRS}
) # Marking them SYSTEM suppresses LLVM’s internal warnings from polluting the build when using /W4.
target_compile_definitions(${PROJECT_NAME} PRIVATE ${LLVM_DEFINITIONS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${LLVM_LIBS})

# C++ Modules MUST go in a FILE_SET
# target_sources(${PROJECT_NAME} PRIVATE
#     FILE_SET CXX_MODULES FILES
#     src/tokeniser.ixx
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
