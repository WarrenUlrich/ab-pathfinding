cmake_minimum_required(VERSION 3.17)

project(ab-pathfinding VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(
    APPEND EXTRA_INCLUDE_DIRS 
        "$ENV{USERPROFILE}/AlpacaBot/Include"
)

list(
    APPEND EXTRA_LIB_DIRS 
        "$ENV{USERPROFILE}/AlpacaBot/Library"
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${EXTRA_INCLUDE_DIRS}
)

add_library(
    ${PROJECT_NAME} INTERFACE
)

find_library(ALPACABOT_LIBRARY NAMES AlpacaLibrary PATHS ${EXTRA_LIB_DIRS} NO_DEFAULT_PATH)
if(NOT ALPACABOT_LIBRARY)
    message(FATAL_ERROR "AlpacaBot library (libAlpacaLibrary.a) not found!")
endif()

add_subdirectory(astar)
add_subdirectory(test-script)