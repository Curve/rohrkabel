cmake_minimum_required(VERSION 3.1)
project(rohrkabel LANGUAGES CXX VERSION 1.2)

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
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wextra -pedantic -fno-strict-aliasing)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------------------------------------------------------
# Disable warnings for GNU-Extensions on Clang because PipeWire makes use of them
# --------------------------------------------------------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-gnu)
endif()

# --------------------------------------------------------------------------------------------------------
# Add source files
# --------------------------------------------------------------------------------------------------------

file(GLOB src "src/*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${src})

# --------------------------------------------------------------------------------------------------------
# Include "include" folder
# --------------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_include_directories(${PROJECT_NAME} PRIVATE "include/rohrkabel")

# --------------------------------------------------------------------------------------------------------
# Link required libraries
# --------------------------------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(PipeWire REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PipeWire_LIBRARIES} pthread)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${PipeWire_INCLUDE_DIRS} ${Spa_INCLUDE_DIRS})

# --------------------------------------------------------------------------------------------------------
# Build examples
# --------------------------------------------------------------------------------------------------------

if (rohrkabel_examples)
    add_subdirectory("examples/roundtrip")
    add_subdirectory("examples/virtual-mic")
    add_subdirectory("examples/thread-loop")
    add_subdirectory("examples/link-app-to-mic")
    add_subdirectory("examples/mute-microphone")
endif()

# --------------------------------------------------------------------------------------------------------
# Install Targets
# --------------------------------------------------------------------------------------------------------

install(DIRECTORY "include/" TYPE INCLUDE)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib INCLUDES DESTINATION include)