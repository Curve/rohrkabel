cmake_minimum_required(VERSION 3.21)
project(rohrkabel LANGUAGES CXX VERSION 7.1)

# --------------------------------------------------------------------------------------------------------
# Library options
# --------------------------------------------------------------------------------------------------------

option(rohrkabel_examples       "Build examples"                             OFF)
option(rohrkabel_sanitizer      "Enable Sanitizer (GCC)"                     OFF)
option(rohrkabel_prefer_remote  "Prefer remote packages over local packages"  ON)

# --------------------------------------------------------------------------------------------------------
# Sync `CPM_DOWNLOAD_ALL` with `rohrkabel_prefer_remote`
# --------------------------------------------------------------------------------------------------------

if (rohrkabel_prefer_remote)
    message(STATUS "[rohrkabel] Avoiding local packages as 'rohrkabel_prefer_remote' is ON")
endif()

set(CPM_DOWNLOAD_ALL ${rohrkabel_prefer_remote})

# --------------------------------------------------------------------------------------------------------
# Enable Sanitizer
# --------------------------------------------------------------------------------------------------------

if (rohrkabel_sanitizer)
  add_compile_options(-fsanitize=address -fsanitize=leak)
  add_link_options(-fsanitize=address -fsanitize=leak)
endif()

# --------------------------------------------------------------------------------------------------------
# Create library
# --------------------------------------------------------------------------------------------------------

add_library(${PROJECT_NAME})
add_library(cr::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

if (PROJECT_IS_TOP_LEVEL)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -pedantic -pedantic-errors -Wfatal-errors)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(${PROJECT_NAME} PUBLIC -Wno-attributes=rk::)
else()
  target_compile_options(${PROJECT_NAME} PUBLIC -Wno-unknown-attributes)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unknown-warning-option -Wno-missing-field-initializers -Wno-cast-function-type)

# --------------------------------------------------------------------------------------------------------
# Disable warnings for GNU-Extensions on Clang because PipeWire makes use of them
# --------------------------------------------------------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-gnu)
endif()

# --------------------------------------------------------------------------------------------------------
# Add source files
# --------------------------------------------------------------------------------------------------------

file(GLOB src 
  "src/*.cpp" 
  "src/spa/*.cpp"
  "src/info/*.cpp"
  "src/events/*.cpp"
)

target_sources(${PROJECT_NAME} PRIVATE ${src})

# --------------------------------------------------------------------------------------------------------
# Include "include" folder
# --------------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC  "include")
target_include_directories(${PROJECT_NAME} PRIVATE "include/rohrkabel")

# --------------------------------------------------------------------------------------------------------
# Setup Dependencies
# --------------------------------------------------------------------------------------------------------

include("cmake/cpm.cmake")

CPMFindPackage(
  NAME           tl-expected
  VERSION        1.1.0
  GIT_REPOSITORY "https://github.com/TartanLlama/expected"
  OPTIONS        "EXPECTED_BUILD_TESTS OFF"
)

CPMFindPackage(
  NAME           ereignis
  VERSION        4.3
  GIT_REPOSITORY "https://github.com/Curve/ereignis"
)

CPMFindPackage(
  NAME           boost_callable_traits
  GIT_TAG        boost-1.86.0
  GIT_REPOSITORY "https://github.com/boostorg/callable_traits"
)

CPMFindPackage(
  NAME           channel
  VERSION        2.3
  GIT_REPOSITORY "https://github.com/Curve/channel"
)

target_link_libraries(${PROJECT_NAME} PUBLIC tl::expected cr::ereignis Boost::callable_traits cr::channel)

# --------------------------------------------------------------------------------------------------------
# Custom Find-Package configurations
# --------------------------------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# --------------------------------------------------------------------------------------------------------
# Setup PipeWire
# --------------------------------------------------------------------------------------------------------

find_package(PipeWire REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PIPEWIRE_LIBRARIES} pthread)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${PIPEWIRE_INCLUDE_DIRS} ${SPA_INCLUDE_DIRS})

# --------------------------------------------------------------------------------------------------------
# Build examples
# --------------------------------------------------------------------------------------------------------

if (rohrkabel_examples)
    add_subdirectory("examples/channels")
    add_subdirectory("examples/roundtrip")
    add_subdirectory("examples/virtual-mic")
    add_subdirectory("examples/mute-microphone")
endif()

# --------------------------------------------------------------------------------------------------------
# Install Targets
# --------------------------------------------------------------------------------------------------------

install(DIRECTORY "include/" TYPE INCLUDE)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib INCLUDES DESTINATION include)
