cmake_minimum_required(VERSION 3.1)
project(rohrkabel LANGUAGES CXX VERSION 1.0)

# --------------------------------------------------------------------------------------------------------
# Library options
# --------------------------------------------------------------------------------------------------------

option(rohrkabel_examples "Build examples" OFF)

# --------------------------------------------------------------------------------------------------------
# Warning for non unix platforms
# --------------------------------------------------------------------------------------------------------

if (NOT UNIX)
    message(FATAL_ERROR "rohrkabel is not supported on non unix platforms")
endif()

# --------------------------------------------------------------------------------------------------------
# Create library
# --------------------------------------------------------------------------------------------------------

add_library(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wextra -pedantic -Wno-gnu)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------------------------------------------------------
# Add source files
# --------------------------------------------------------------------------------------------------------

file(GLOB src "src/*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${src})

# --------------------------------------------------------------------------------------------------------
# Include "include" folder
# --------------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC "include")

# --------------------------------------------------------------------------------------------------------
# Link required libraries
# --------------------------------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(PipeWire REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PipeWire_LIBRARIES})
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${PipeWire_INCLUDE_DIRS} ${Spa_INCLUDE_DIRS})

# --------------------------------------------------------------------------------------------------------
# Build examples
# --------------------------------------------------------------------------------------------------------

if (rohrkabel_examples)
    add_subdirectory("examples/roundtrip")
endif()