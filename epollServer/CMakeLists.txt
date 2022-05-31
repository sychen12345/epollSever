aux_source_directory(. DIR_HELLO_SRCS)
include_directories(${PROJECT_SOURCE_DIR}/base/cMemory)
include_directories(${PROJECT_SOURCE_DIR}/base/cLock)
include_directories(${PROJECT_SOURCE_DIR}/connectPool)
include_directories(${PROJECT_SOURCE_DIR}/eventCallBack)

add_library(epollServer ${DIR_HELLO_SRCS})