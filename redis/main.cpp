#include <iostream>
#include <hiredis/hiredis.h>
#include <stdlib.h>
#include <string>

#define HOST_IP "127.0.0.1"
#define BIND_PORT 6379

typedef struct contextInfo
{
    const char *key;
    const char *value;
}contextInfo_t;

contextInfo_t g_info[] =
{
    {"article:10086:title", "message"},
    {"article:10086:content", "hello world"},
    {"article:10086:author", "doudou"},
    {"article:10086:create_at", "1590216574.123456"},
};

typedef struct HashContextInfo
{
    const char *key;
    contextInfo_t context[4];
}HashContextInfo_t;

HashContextInfo_t g_hashContext = 
{
    
    "article:10086", 
    
    {
        {"title", "message"},
        {"content", "hello world"}, 
        {"author", "doudou"}, 
        {"create_at", "1590216574.123456"}
    }
    
};

int main()
{
    //Á¬½Ó
    redisContext *context = redisConnect(HOST_IP, BIND_PORT);
    if (context == NULL || context->err)
    {
        if (context)
        {
            std::cout << context->errstr << std::endl;
        }
        else
        {
            std::cout << "redisConnect error" << std::endl; 
        }
        
        exit(EXIT_FAILURE);
    }
    
    // redisReply *reply = (redisReply*)redisCommand(context, "");
    // std::cout << "type : " << reply->type << std::endl;
    
    // if (reply->type == REDIS_REPLY_STATUS)
    // {
        // std::cout << "auth ok" << std::endl;
    // }
    
#if 0
    const char *key = "test_c_string";
    const char *val = "Hello World";
    redisReply *reply = (redisReply*)redisCommand(context, "SET %s %s", key, val);
    std::cout << "type : " << reply->type << std::endl;
    
    if (reply->type == REDIS_REPLY_STATUS)
    {
        printf("SET %s %s\n", key, val);
    }
    
    freeReplyObject(reply);

    //Get key
    reply = (redisReply*)redisCommand(context, "GET %s", key);
    if (reply->type == REDIS_REPLY_STRING)
    {
        std::cout << "GET str " << reply->str << std::endl;
        std::cout << "GET len " << reply->len << std::endl;
    }
    
    freeReplyObject(reply);

    //append key value
    std::string strAppend(",I am your GOD");
    reply = (redisReply*)redisCommand(context, "APPEND %s %s", key, strAppend.c_str());
    if (reply->type == REDIS_REPLY_INTEGER)
    {
        std::cout << "APPEND " << key << " " << strAppend << std::endl;
    }
    
    freeReplyObject(reply);

    /* GET key */
    reply = (redisReply*)redisCommand(context, "GET %s", key);
    if (reply->type == REDIS_REPLY_STRING)
    {
        std::cout << "GET " << reply->str << std::endl;
    }
    
    freeReplyObject(reply);
#endif

    //MSET string
    redisReply *reply = (redisReply*)redisCommand(context, "MSET %s %s %s %s %s %s %s %s",
    g_info[0].key, g_info[0].value,
    g_info[1].key, g_info[1].value,
    g_info[2].key, g_info[2].value,
    g_info[3].key, g_info[3].value);
    
    if (reply->type == REDIS_REPLY_STATUS)
    {
        std::cout << "MSET success" << std::endl;
    }
    else
    {
        std::cout << "MSET error" << std::endl;
    }
    
    freeReplyObject(reply);

    //get string
    reply = (redisReply*)redisCommand(context, "MGET %s %s %s %s", g_info[0].key, g_info[1].key, g_info[2].key, g_info[3].key);
    
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (int i = 0; i < reply->elements; ++i)
        {
            redisReply* tmpReply = (redisReply*)reply->element[i];
            std::cout << "key : " << g_info[i].key; 
            std::cout << ", value : " << tmpReply->str << std::endl;
        }
    }
    
    freeReplyObject(reply);
    
    
    //HMSET string
    reply = (redisReply*)redisCommand(context, "HMSET %s %s %s %s %s %s %s %s %s",
    g_hashContext.key,
    g_hashContext.context[0].key,g_hashContext.context[0].value,
    g_hashContext.context[1].key,g_hashContext.context[1].value,
    g_hashContext.context[2].key,g_hashContext.context[2].value,
    g_hashContext.context[3].key,g_hashContext.context[3].value);
    
    if (reply->type == REDIS_REPLY_STATUS)
    {
        std::cout << "HMSET success" << std::endl;
    }
    else
    {
        std::cout << "HMSET error" << std::endl;
    }
    
    freeReplyObject(reply);

    //HMGET string
    reply = (redisReply*)redisCommand(context, "HMGET %s %s %s %s %s", 
    g_hashContext.key, 
    g_hashContext.context[0].key,
    g_hashContext.context[1].key,
    g_hashContext.context[2].key,
    g_hashContext.context[3].key
    );
    
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (int i = 0; i < reply->elements; ++i)
        {
            redisReply* tmpReply = (redisReply*)reply->element[i];
            std::cout << "key : " << g_hashContext.key; 
            std::cout << ", value : " << tmpReply->str << std::endl;
        }
    }
    
    freeReplyObject(reply);
    return 0;
}