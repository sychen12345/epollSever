aux_source_directory(. DIR_HELLO_SRCS)
include_directories(${PROJECT_SOURCE_DIR}/base/cMemory)
include_directories(${PROJECT_SOURCE_DIR}/base/cLock)

add_library(connectPool ${DIR_HELLO_SRCS})