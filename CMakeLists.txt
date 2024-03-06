cmake_minimum_required(VERSION 3.17)

project(ab-pathfinding VERSION 1.0.0 LANGUAGES CXX)

add_subdirectory(astar)

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

# add_library(
#     ${PROJECT_NAME} STATIC
#         src/pathfinding.cpp
#         src/bank.cpp
#         src/navigation_link.cpp
#         src/banks/bank_booth_bank.cpp
#         src/banks/banker_bank.cpp
# )

add_library(
    ${PROJECT_NAME} INTERFACE
)

target_include_directories(
    ${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${EXTRA_INCLUDE_DIRS}
)

find_library(ALPACABOT_LIBRARY NAMES AlpacaLibrary PATHS ${EXTRA_LIB_DIRS} NO_DEFAULT_PATH)
if(NOT ALPACABOT_LIBRARY)
    message(FATAL_ERROR "AlpacaBot library (libAlpacaLibrary.a) not found!")
endif()


target_link_libraries(
    ${PROJECT_NAME} INTERFACE 
        astar
        ${ALPACABOT_LIBRARY}
)

add_subdirectory(test-script)
add_subdirectory(collision-cacher)