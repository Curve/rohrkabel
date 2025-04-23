cmake_minimum_required(VERSION 3.25)
project(volume LANGUAGES CXX VERSION 1.0)

# --------------------------------------------------------------------------------------------------------
# Create library
# --------------------------------------------------------------------------------------------------------

add_executable(${PROJECT_NAME} "main.cpp")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)


# --------------------------------------------------------------------------------------------------------
# Supply rohrkabel if example is built stand-alone
# --------------------------------------------------------------------------------------------------------

include("../../cmake/cpm.cmake")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    CPMFindPackage(
        NAME           rohrkabel
        GIT_TAG        master
        GIT_REPOSITORY "https://github.com/Curve/rohrkabel"
    )
endif()
    
# --------------------------------------------------------------------------------------------------------
# Link required libraries
# --------------------------------------------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} PRIVATE rohrkabel)
