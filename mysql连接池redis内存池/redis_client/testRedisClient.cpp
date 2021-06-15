/*
 * @Author: 零声学院 Darren老师 QQ:326873713
 * @Date: 2019-12-09 17:31:53
 * @LastEditTime: 2020-05-27 20:25:35
 * @LastEditors: Please set LastEditors
 * @Description: Redis客户端接口
 * @FilePath: testRedisClient.cpp
 */
#include <sstream>
#include <vector>
#include <unistd.h>
#include "RedisDBInterface.h"
using namespace std;
#define REDIS_TEST_COUNT 1000
#define LOG_INFO(msg, ...)      \
    printf(msg, ##__VA_ARGS__); \
    printf("\n")
#define ASSERT_REDIS_RET(ret, cmd, key) \
    if (!(ret))                         \
        printf("%s(%d) cmd %s %s operate failed\n", __FUNCTION__, __LINE__, cmd, key);

static string int2string(uint32_t user_id)
{
    stringstream ss;
    ss << user_id;
    return ss.str();
}

static uint64_t getTickCount()
{
    struct timeval tval;
    uint64_t ret_tick;

    gettimeofday(&tval, NULL);

    ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
    return ret_tick;
}

// 测试string操作
void testString(CRedisDBInterface *pRedisClient)
{
    LOG_INFO("\n------------- testString -------------");
    REDISStatus ret = REDISStatus::KVDB_ERROR;
    ret = pRedisClient->FlushDB();
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "FLUSHDB", "");
    string key = "teacher";
    string value = "darren";
    ret = pRedisClient->Set(key, value);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "SET", key.c_str());
    ret = pRedisClient->Get(key, value);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "GET", key.c_str());
    ret = pRedisClient->SetEX(key, value, 2);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "SETEX", key.c_str());
    bool isExit = false;
    ret = pRedisClient->IsStringKeyExits(key, isExit);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "EXISTS", key.c_str());
    if (!isExit)
        LOG_INFO("SETEX failed 1");
    sleep(3);
    isExit = false;
    ret = pRedisClient->IsStringKeyExits(key, isExit);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "EXISTS", key.c_str());
    if (isExit)
        LOG_INFO("SETEX failed 2");

    LOG_INFO("MSET MGET");
    std::vector<std::string> strKeys; 
	std::vector<std::string> strVals;
    strKeys.push_back("user:1:name");
    strVals.push_back("darren");
    strKeys.push_back("user:1:sex");
    strVals.push_back("man");
    ret = pRedisClient->Mset(strKeys, strVals);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "MSET", ""); 
    strVals.clear();
    ret = pRedisClient->Mget(strKeys, strVals);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "MGET", "");
    LOG_INFO("MGET 1"); 
    for (int i = 0; i < strKeys.size(); i++)
    {
        LOG_INFO("%s - %s,", strKeys[i].c_str(), strVals[i].c_str());
    }

    strVals.clear();
    strKeys.push_back("user:1:email");
    strKeys.push_back("user:1:sex");
    ret = pRedisClient->Mget(strKeys, strVals);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "MGET", key.c_str());
    LOG_INFO("MGET 2"); 
    for (int i = 0; i < strKeys.size(); i++)
    {
        // strVals[i].size() 为0的时候说明没有取到对应key的数据
        LOG_INFO("%s - %s[%ld]", strKeys[i].c_str(), strVals[i].c_str(), strVals[i].size());
    }

}

void testHash(CRedisDBInterface *pRedisClient)
{
    LOG_INFO("\n------------- testHash -------------");
    REDISStatus ret = REDISStatus::KVDB_ERROR;
    ret = pRedisClient->FlushDB();
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "FLUSHDB", "");
    string key = "article:10086";
    string fieldTitle = "title";
    string valueTitle = "message";
    string fieldContent = "content";
    string valueContent = "hello world";
    string fieldAuthor = "author";
    string valueAuthor = "darren";

    // 验证HSET HGET
    ret = pRedisClient->Hset(key, fieldTitle, valueTitle);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK==ret , "HSET", key.c_str());
    ret = pRedisClient->Hset(key, fieldContent, valueContent);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK==ret , "HSET", key.c_str());
    ret = pRedisClient->Hset(key, fieldAuthor, valueAuthor);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK==ret , "HSET", key.c_str());

    string tempTitle, tempContent, tempAuthor;
    ret = pRedisClient->Hget(key, fieldTitle, tempTitle);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK==ret , "HGET", key.c_str());
    ret = pRedisClient->Hget(key, fieldContent, tempContent);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK==ret , "HGET", key.c_str());
    ret = pRedisClient->Hget(key, fieldAuthor, tempAuthor);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK==ret , "HGET", key.c_str());
    LOG_INFO("HGET %s, %s, %s\n", tempTitle.c_str(), tempContent.c_str(), tempAuthor.c_str());
    // 验证HDEL
    ret = pRedisClient->Hdel(key, fieldTitle);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK==ret , "HDEL", key.c_str());
    ret = pRedisClient->Hdel(key, fieldContent);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK==ret , "HDEL", key.c_str());
    ret = pRedisClient->Hdel(key, fieldAuthor);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK==ret , "HDEL", key.c_str());

    // 验证HMSET
    std::vector<std::pair<std::string, std::string>> hashItemsPair;
    hashItemsPair.push_back(std::pair<std::string, std::string>(fieldTitle, valueTitle));
    hashItemsPair.push_back(std::pair<std::string, std::string>(fieldContent, valueContent));
    hashItemsPair.push_back(std::pair<std::string, std::string>(fieldAuthor, valueAuthor));
    ret = pRedisClient->HMset(key, hashItemsPair);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "HMSET", key.c_str());

    LOG_INFO("HMGET");
    std::vector<std::string> hashItemFields;
    std::vector<std::string> hashItemValues;
    hashItemFields.push_back(fieldTitle);
    hashItemFields.push_back(fieldContent);
    hashItemFields.push_back(fieldAuthor);
    hashItemFields.push_back("darren");     // 故意插入一个不存在的field
    ret = pRedisClient->HMget(key, hashItemFields, hashItemValues);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "HMGET", key.c_str());
    for (int i = 0; i < hashItemFields.size(); i++)
    {
        LOG_INFO("%s - %s", hashItemFields[i].c_str(), hashItemValues[i].c_str());
    }

    LOG_INFO("HGETALL");
    std::map<std::string, std::string> mapFileValue;
    ret = pRedisClient->HgetAll(key, mapFileValue);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "HGETALL", key.c_str());

    int nIndex = 0;
    for (auto &itor : mapFileValue)
    {
        nIndex++;
        LOG_INFO("mapFileValue: [%d]itor.first:%s, itor.second:%s", 
            nIndex, itor.first.c_str(), itor.second.c_str());
    }

    LOG_INFO("HSCAN");
    hashItemsPair.clear();
    for (int i = 0; i < 10; i++)// 字段量少的时候 count不起作用，可以测试下插入100、1000、10000个字段
    {
        hashItemsPair.push_back(std::pair<std::string, std::string>("field-" + int2string(i), "darren-" + int2string(i)));
    }
    ret = pRedisClient->HMset(key, hashItemsPair);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "HMSET", key.c_str());
    std::list<std::string> hashItems;
    std::list<std::string> hashItemsValue;
    std::string strHashItemMatch = "field*";
    int count = 5;
    int cursor = 0;
    int items_count = 0;
    while (true) {
        // 字段量少的时候 count不起作用，可以测试下插入100、1000、10000个字段
        ret = pRedisClient->HScan(key, strHashItemMatch, count, cursor, hashItems, hashItemsValue);
        ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "HSCAN", key.c_str());
        auto items = hashItems.begin();
        auto itemsValue = hashItemsValue.begin();
        for (; items != hashItems.end();) {
            LOG_INFO("[%d] %s - %s", cursor, items->c_str(), itemsValue->c_str());
            items++;
            itemsValue++;
        }
        LOG_INFO("Hscan %ld items,cursor:%d ----------", hashItems.size(), cursor);
        items_count += hashItems.size();
        hashItems.clear();
        hashItemsValue.clear();
        if(0 == cursor) {       // 游标返回0的时候遍历结束
            LOG_INFO("Hscan finish, hashItems:%d", items_count);
            break;
        }
        
    }
}

void testList(CRedisDBInterface *pRedisClient)
{
    LOG_INFO("\n------------- testList -------------");
    REDISStatus ret = REDISStatus::KVDB_ERROR;
    ret = pRedisClient->FlushDB();
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "FLUSHDB", "");
    string key = "list";
    for(int i = 0; i < 10; i++) {
        string value =  "darren-" + int2string(i);
        ret = pRedisClient->ListLPush(key, value);
        ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "LPUSH", key.c_str());
    }

    for(int i = 0; i < 10; i++) {
        string value =  "darren-" + int2string(i+10);
        ret = pRedisClient->ListRPush(key, value);
        ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "RPUSH", key.c_str());
    }
    int64_t nListSize = 0;
    ret = pRedisClient->ListSize(key, nListSize);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "LLEN", key.c_str());
    LOG_INFO("list size:%ld", nListSize);

    int index = 10;
    string strItem;
    ret = pRedisClient->ListIndexElement(key, index, strItem);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "LINDEX", key.c_str());
    LOG_INFO("LINDEX index[%d]:%s", index, strItem.c_str());

    // 出队列
    strItem.clear();
    ret = pRedisClient->ListRPop(key, strItem);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "RPOP", key.c_str());
    LOG_INFO("RPOP %s", strItem.c_str());
    strItem.clear();
    ret = pRedisClient->ListLPop(key, strItem);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "LPOP", key.c_str());
    LOG_INFO("LPOP %s", strItem.c_str());

    strItem.clear();
    ret = pRedisClient->ListIndexElement(key, index, strItem);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "LINDEX", key.c_str());
    LOG_INFO("LINDEX index[%d]:%s", index, strItem.c_str());
}

void testSet(CRedisDBInterface *pRedisClient)
{
    LOG_INFO("\n------------- testSet -------------");
    REDISStatus ret = REDISStatus::KVDB_ERROR;
    ret = pRedisClient->FlushDB();
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "FLUSHDB", "");

    string key = "set";
    for(int i = 0; i < 10; i++) {
        string value =  "darren-" + int2string(i);
        ret = pRedisClient->Sadd(key, value);
        ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "SADD", key.c_str());
    }
    int64_t nSetItems = 0;
    ret = pRedisClient->Scard(key, nSetItems);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "SCARD", key.c_str());
    LOG_INFO("SCARD %ld", nSetItems);

    std::set<std::string> rgItem;
    ret = pRedisClient->Smembers(key, rgItem);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "SMEMBERS", key.c_str());
    LOG_INFO("SMEMBERS size:%ld", rgItem.size());
    for (auto it = rgItem.begin(); it != rgItem.end(); it++)
    {
        LOG_INFO("%s",it->c_str());
    }

    string tempItem = "darren-0";
    ret = pRedisClient->Srem(key, tempItem);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "SREM", key.c_str());
    rgItem.clear();     // 需要清空原来的数据，接口内部不会清除数据
    ret = pRedisClient->Smembers(key, rgItem);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "SMEMBERS", key.c_str());
    LOG_INFO("SMEMBERS size:%ld", rgItem.size());
    for (auto it = rgItem.begin(); it != rgItem.end(); it++)
    {
        LOG_INFO("%s",it->c_str());
    }
}

void testZset(CRedisDBInterface *pRedisClient)
{
    LOG_INFO("\n------------- testZset -------------");
    REDISStatus ret = REDISStatus::KVDB_ERROR;
    ret = pRedisClient->FlushDB();
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "FLUSHDB", "");

    string key = "zset";
    for(int i = 0; i < 10; i++) {
        string value =  "darren-" + int2string(i);
        ret = pRedisClient->Zadd(key, i, value);
        ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "ZADD", key.c_str());
    }
    int64_t nSetItems = 0;
    ret = pRedisClient->ZCard(key, nSetItems);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "ZCARD", key.c_str());
    LOG_INFO("ZCARD %ld", nSetItems);

    // 由小到大排序
    std::vector<std::string> topicItem;
    int64_t from = 0;
    int64_t to = -1;
    std::vector<std::string> scoreItem;
    ret = pRedisClient->ZRangeWithScore(key, topicItem, from, to, scoreItem);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "ZRANGE", key.c_str());
    LOG_INFO("ZRANGE size:%ld", topicItem.size());
    auto itTopic = topicItem.begin();
    auto itScore = scoreItem.begin();
    for (; itTopic != topicItem.end(); itTopic++, itScore++)
    {
        LOG_INFO("%s - %s",itTopic->c_str(), itScore->c_str());
    }

    string tempItem = "darren-0";
    ret = pRedisClient->Zrem(key, tempItem);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "ZREM", key.c_str());
    topicItem.clear();
    scoreItem.clear();
    ret = pRedisClient->ZRangeWithScore(key, topicItem, from, to, scoreItem);
    ASSERT_REDIS_RET(REDISStatus::KVDB_OK == ret, "ZRANGE", key.c_str());
    LOG_INFO("ZRANGE size:%ld", topicItem.size());
    itTopic = topicItem.begin();
    itScore = scoreItem.begin();
    for (; itTopic != topicItem.end(); itTopic++, itScore++)
    {
        LOG_INFO("%s - %s",itTopic->c_str(), itScore->c_str());
    }
}

int main(void)
{
    string redis_ip = "127.0.0.1";
    int redis_port = 6379;
    CRedisDBInterface *pRedisClient = new CRedisDBInterface(redis_ip, redis_port);
    if (!pRedisClient->ConnectDB())
    {
        LOG_INFO("connection redis db failed\n");
        delete pRedisClient;
        return -1;
    }
    string key = "teacher";
    string value = "darren";
    pRedisClient->Set(key, value);
    value = "";
    pRedisClient->Get(key, value);
    LOG_INFO("Get value:%s\n", value.c_str());

    // testString(pRedisClient);
    testHash(pRedisClient);
    // testList(pRedisClient);
    // testSet(pRedisClient);
    // testZset(pRedisClient);
    
    LOG_INFO("test finish\n");
    delete pRedisClient;

    return 0;
}