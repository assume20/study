# redis客户端编程
支持
- string
- hash
- list
- set
- zset


# 编译步骤
mkdir build

cd build

cmake ..

make

则在build目录看到执行文件testRedisClient

# 说明
- 大家增加了新的指令实现可以合并进来
- 主要的文件：RedisDBInterface.cpp，RedisDBInterface.h和testRedisClient.cpp，其余为hiredis的代码，为了更方便查看redis的源码所以放在了一起。
