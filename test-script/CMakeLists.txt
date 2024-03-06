cmake_minimum_required(VERSION 3.26)

project(pathfinding-test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LINKER_FLAGS "-Os -s -m32 -static-libgcc -static-libstdc++ -static")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

if (NOT DEFINED ALPACABOT_DIR)
  set(ALPACABOT_DIR $ENV{USERPROFILE}\\AlpacaBot)
endif()

if (NOT DEFINED ALPACABOT_INCLUDE_DIR)
  set(ALPACABOT_INCLUDE_DIR ${ALPACABOT_DIR}\\Include)
endif()

if (NOT DEFINED ALPACABOT_LIB_DIR)
  set(ALPACABOT_LIB_DIR ${ALPACABOT_DIR}\\Library)
endif()

if (NOT DEFINED ALPACABOT_SCRIPT_DIR)
  set(ALPACABOT_SCRIPT_DIR ${ALPACABOT_DIR}\\Scripts\\Local)
endif()


set(SOURCE_FILES
    src/main.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${ALPACABOT_SCRIPT_DIR}/${PROJECT_NAME}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${ALPACABOT_INCLUDE_DIR}
)

target_link_directories(${PROJECT_NAME} PRIVATE
  ${ALPACABOT_LIB_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  AlpacaLibrary
  ab-pathfinding
)
