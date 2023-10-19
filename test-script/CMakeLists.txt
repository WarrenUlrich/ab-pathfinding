cmake_minimum_required(VERSION 3.15)

project(ab-pathfinding-test VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(
  ${PROJECT_NAME} SHARED 
    src/main.cpp
)

target_link_libraries(
  ${PROJECT_NAME}
    PRIVATE
      ${ALPACABOT_LIBRARY}
      astar
      ab-pathfinding
)

# Set the output directory for the shared library
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "$ENV{USERPROFILE}/AlpacaBot/Scripts/Local/${PROJECT_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "$ENV{USERPROFILE}/AlpacaBot/Scripts/Local/${PROJECT_NAME}"
    ARCHIVE_OUTPUT_DIRECTORY "$ENV{USERPROFILE}/AlpacaBot/Scripts/Local/${PROJECT_NAME}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
