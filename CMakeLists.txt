cmake_minimum_required(VERSION 3.25)
project(rohrkabel LANGUAGES CXX VERSION 9.0.0)

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
# Create library
# --------------------------------------------------------------------------------------------------------

add_library(${PROJECT_NAME})
add_library(cr::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

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

target_include_directories(${PROJECT_NAME} PRIVATE "include/rohrkabel")

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

# --------------------------------------------------------------------------------------------------------
# Setup Dependencies
# --------------------------------------------------------------------------------------------------------

include("cmake/cpm.cmake")

CPMFindPackage(
  NAME           coco
  VERSION        3.1.0
  GIT_REPOSITORY "https://github.com/Curve/coco"
)

CPMFindPackage(
  NAME           ereignis
  VERSION        5.0.0
  GIT_REPOSITORY "https://github.com/Curve/ereignis"
)

CPMFindPackage(
  NAME           channel
  VERSION        3.0.1
  GIT_REPOSITORY "https://github.com/Curve/channel"
)

target_link_libraries(${PROJECT_NAME} PUBLIC cr::ereignis cr::channel cr::coco)

# --------------------------------------------------------------------------------------------------------
# Custom Find-Package configurations
# --------------------------------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# --------------------------------------------------------------------------------------------------------
# Setup PipeWire
# --------------------------------------------------------------------------------------------------------

find_package(PipeWire REQUIRED)

target_link_libraries(${PROJECT_NAME}             PUBLIC ${PIPEWIRE_LIBRARIES} pthread)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${PIPEWIRE_INCLUDE_DIRS} ${SPA_INCLUDE_DIRS})

# --------------------------------------------------------------------------------------------------------
# Build examples
# --------------------------------------------------------------------------------------------------------

if (rohrkabel_examples)
    add_subdirectory("examples/volume")
    add_subdirectory("examples/channels")
    add_subdirectory("examples/roundtrip")
    add_subdirectory("examples/virtual-mic")
    add_subdirectory("examples/mute-microphone")
endif()

# --------------------------------------------------------------------------------------------------------
# Package Config
# --------------------------------------------------------------------------------------------------------

include("cmake/cpm.cmake")

CPMFindPackage(
  NAME           PackageProject
  VERSION        1.13.0
  GIT_REPOSITORY "https://github.com/TheLartians/PackageProject.cmake"
)

packageProject(
  NAMESPACE cr
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  DEPENDENCIES "coco 3.1.0;ereignis 5.0.0;channel 3.0.1"
)
