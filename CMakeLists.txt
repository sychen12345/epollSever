cmake_minimum_required(VERSION 3.20)
project(epollTcp)

set(CMAKE_CXX_STANDARD 14)
include_directories(${PROJECT_SOURCE_DIR}/connectPool)
include_directories(${PROJECT_SOURCE_DIR}/listenNet)
include_directories(${PROJECT_SOURCE_DIR}/eventCallBack)
include_directories(${PROJECT_SOURCE_DIR}/epollServer)

include_directories(${PROJECT_SOURCE_DIR}/base/cMemory)
include_directories(${PROJECT_SOURCE_DIR}/base/cLock)

add_subdirectory(${PROJECT_SOURCE_DIR}/epollServer)
add_subdirectory(${PROJECT_SOURCE_DIR}/eventCallBack)
add_subdirectory(${PROJECT_SOURCE_DIR}/connectPool)
add_subdirectory(${PROJECT_SOURCE_DIR}/base)
add_subdirectory(${PROJECT_SOURCE_DIR}/listenNet)
add_executable(epollTcp main.cpp)
target_link_libraries(${PROJECT_NAME} epollServer eventCallBack netListener connectPool cLock cMemory pthread)
