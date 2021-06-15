cmake_minimum_required(VERSION 2.8)
PROJECT(testRedisClient)

SET(SRC_LIST
    async.c 
    dict.c
    hiredis.c
    net.c
    read.c
    sds.c
    RedisDBInterface.cpp
)

SET(EXECUTABLE_OUTPUT_PATH  ./)




ADD_EXECUTABLE(testRedisClient testRedisClient.cpp ${SRC_LIST})

TARGET_LINK_LIBRARIES(testRedisClient pthread) 