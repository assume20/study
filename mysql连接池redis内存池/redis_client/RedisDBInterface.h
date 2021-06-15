/*
 * @Author: 零声学院 Darren老师 QQ:326873713
 * @Date: 2019-12-09 17:31:53
 * @LastEditTime: 2020-05-27 20:11:58
 * @LastEditors: Please set LastEditors
 * @Description: Redis客户端接口
 * @FilePath: RedisDBInterface.h
 */
#ifndef __REDIS_DB_INTERFACE_IINCLUDE_H__
#define __REDIS_DB_INTERFACE_IINCLUDE_H__

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <string>
#include <map>

#include "hiredis.h"


#define MAX_SCORE   "+inf"
#define MIN_SCORE   "-inf"

enum class REDISStatus
{
	KVDB_OK = 0,
	KVDB_RNULL = -1,
	KVDB_ERROR = -2
};

class CRedisDBInterface
{
public:
	CRedisDBInterface()
		:m_pRedisDBContex(NULL)
	{

	}


    CRedisDBInterface(const std::string &strIp, int32_t nPort)
        :m_pRedisDBContex(NULL),
		m_strIp(strIp),
		m_nPort(nPort)
    {
    }

    
	virtual ~CRedisDBInterface();
	//-----------------检查当前数据库是否断开连接-------------
	virtual bool IsConnected();
	virtual bool ConnectDB();
	virtual bool ConnectDB(const std::string &strIp, int32_t nPort);
	virtual bool ReConnectDB();
	virtual REDISStatus FlushDB();

	virtual REDISStatus DelKey(const std::string &strKey);
	virtual REDISStatus Keys(const std::string & pattern, std::list<std::string> &keyItems);
	virtual REDISStatus Expire(const std::string &strName, uint64_t nNum);
	// 实际是用INCR做的
	virtual REDISStatus GetSeqID(const std::string &strKey, int64_t& nRet);

	//-----------------队列操作------------------------------
	virtual REDISStatus ListRPop(const std::string &strName, std::string &strItem);
	virtual REDISStatus ListLPop(const std::string &strName, std::string &strItem);
	virtual REDISStatus ListLPush(const std::string &strName, const std::string &strItem);
	virtual REDISStatus ListRPush(const std::string &strName, const std::string &strItem);
	virtual REDISStatus ListIndexElement(const std::string &strName, int32_t nIndex, std::string &strItem);
	virtual REDISStatus ListSize(const std::string &strName, int64_t& nRet);

	//------------------字符串操作--------------------------
    virtual REDISStatus IsStringKeyExits(const std::string &strKey, bool &bIsExit);
    virtual REDISStatus Set(const std::string &strKey, const std::string &strVal);
    virtual REDISStatus Get(const std::string &strKey, std::string &strVal);
    virtual REDISStatus SetEX(const std::string &strKey, const std::string &strVal, uint64_t nNum);
	virtual REDISStatus IncrSeq(const std::string &strName, uint64_t &unItem);
	virtual REDISStatus IncrSeqBy(const std::string &strName, uint64_t &unItem, uint64_t nNum);
	virtual REDISStatus Mset(const std::vector<std::string> &strKeys, const std::vector<std::string> &strVals);
	virtual REDISStatus Mget(const std::vector<std::string> &strKeys, std::vector<std::string> &strVals);

	//-----------------无序集合操作-----------------------------
	virtual REDISStatus Scard(const std::string &strName, int64_t& nRet);
	virtual REDISStatus Smembers(const std::string &strName, std::set<std::string> &rgItem);
	virtual REDISStatus Sismember(const std::string &strName, const std::string &strItem, bool &bIsExit);
	virtual REDISStatus Sadd(const std::string &strKey, const  std::string &strItem);
	virtual REDISStatus Srem(const std::string &strKey, const std::string &strItem);
	
	//-----------------有序集合操作----------------------------- 
	virtual REDISStatus Zadd(const std::string &strKey, int64_t score, const std::string &strItem);
	virtual REDISStatus Zrem(const std::string &strKey, const std::string &strItem);
	virtual REDISStatus ZScore(const std::string &strKey, const  std::string &strItem, int64_t& nRet);
	virtual REDISStatus ZCount(const std::string &strKey, int64_t from, const char *to, int64_t& nCount);
	virtual REDISStatus ZCard(const std::string &strKey, int64_t& nLen);

	//---------------------------key score value-----------
	virtual REDISStatus ZRangeWithScore(const std::string &strKey, std::vector<std::string> &topicItem, int64_t from, int64_t to, std::vector<std::string> &scoreItem);
	virtual REDISStatus ZRangeByScore(const std::string &strKey, int64_t from, const char *to, std::vector<std::string> &memberItem);
    virtual REDISStatus ZRangeByScoreWithScores(const std::string &strKey, int64_t from, const char *to, std::vector<std::string> &memberItem, std::vector<std::string> &scoreItem);
    virtual REDISStatus ZRangeByScoreAndLimitWithScore(const std::string &strKey, int64_t nStart, int64_t nEnd, int64_t nLimit, std::vector<std::string> &memberItem, std::vector<std::string> &scoreItem);
    virtual REDISStatus ZReverRangeByScoreAndLimitWithScores(const std::string &strKey, int64_t nStart, int64_t nLimit, std::vector<std::string> &memberItem, std::vector<std::string> &scoreItem);
 	virtual REDISStatus ZReverRangeByScoreAndLimit(const std::string &strKey, int64_t nStart, int64_t nLimit, std::vector<std::string> &memberItem); 
	virtual REDISStatus ZSetGetPeerMsgs(const std::string &strKey, std::vector<std::string> &msgs, int32_t& nTotalSize);	// range scope 0 -1
	virtual REDISStatus ZremByScore(const std::string &strKey, int64_t scorefrom, int64_t scoreto);
	virtual REDISStatus ZSetIncrby(const std::string &strKey, const std::string &strMember, const std::string &strScore, int64_t& nRet);
	
	//-----------------哈希操作---key field value---------
	virtual REDISStatus Hset(const std::string &strKey, const std::string &strField, const std::string &strItem);
    virtual REDISStatus Hget(const std::string &strKey, const std::string &strField, std::string &strItem);
	virtual REDISStatus HMset(const std::string &strKey,const std::vector<std::pair<std::string,std::string> > &hashItemsPair);
    virtual REDISStatus HMget(const std::string &strKey,const std::vector<std::string> &hashItemFields,std::vector<std::string> &hashItemValues);
	virtual REDISStatus Hdel(const std::string &strKey, const std::string &strField);
    virtual REDISStatus HgetAll(const std::string &strKey, std::map<std::string, std::string> &mapFileValue);
	virtual REDISStatus HIncrby(const std::string &strKey, const std::string &strField, int64_t &unItem, int64_t val = 1);
	virtual REDISStatus HScan(const std::string &strKey, const std::string &strHashItemMatch, int count,
		 int& cursor, std::list<std::string>& hashItems, std::list<std::string>& hashItemsValue);
;

private:
	REDISStatus CheckReply(const std::string & strFunName, redisReply * pReply);
	REDISStatus FlushDBPrivate(const std::string &strKeyName, const std::string &strFunName);
    REDISStatus IsKeyExits(const std::string &strKey, bool &bIsExit, const std::string &strFunName);

    REDISStatus IncrSeqPrivate(const std::string &strName, uint64_t &unItem, const std::string &strFunName);
    REDISStatus IncrSeqByPrivate(const std::string &strName, uint64_t &unItem, uint64_t nNum, const std::string &strFunName);
    REDISStatus KeysPrivate(const std::string & pattern, std::list<std::string> &keyItems, const std::string &strFunName);
    REDISStatus ExpirePrivate(const std::string &strName, uint64_t nNum, const std::string &strFunName);

	//队列操作
    REDISStatus ListRPopPrivate(const std::string &strName, std::string &strItem, const std::string &strFunName);
    REDISStatus ListLPopPrivate(const std::string &strName, std::string &strItem, const std::string &strFunName);
    REDISStatus ListLPushPrivate(const std::string &strName, const std::string &strItem, const std::string &strFunName);
    REDISStatus ListIndexElementPrivate(const std::string &strName, int32_t nIndex, std::string &strItem, const std::string &strFunName);
    REDISStatus ListRPushPrivate(const std::string &strName, const std::string &strItem, const std::string &strFunName);
    REDISStatus ListSizePrivate(const std::string &strName, int64_t& nRet, const std::string &strFunName);
    
	REDISStatus SetPrivate(const std::string &strKey, const std::string &strVal, const std::string &strFunName);
    REDISStatus GetPrivate(const std::string &strKey, std::string &strVal, const std::string &strFunName);
    REDISStatus SetEXPrivate(const std::string &strKey, const std::string &strVal, uint64_t nNum, const std::string &strFunName);
	REDISStatus MsetPrivate(const std::vector<std::string> &strKeys, const std::vector<std::string> &strVals, 	\
							const std::string &strFunName);
	REDISStatus MgetPrivate(const std::vector<std::string> &strKeys, \
							std::vector<std::string> &strVals, const std::string &strFunName);
    REDISStatus ScardPrivate(const std::string &strName, int64_t& nRet, const std::string &strFunName);
    REDISStatus SmembersPrivate(const std::string &strName, std::set<std::string> &rgItem, const std::string &strFunName);
    REDISStatus SismemberPrivate(const std::string &strName, const std::string &strItem, bool &bIsExit, const std::string &strFunName);
	

    REDISStatus SaddPrivate(const std::string &strKey, const  std::string &strItem, const std::string &strFunName);
    REDISStatus SremPrivate(const std::string &strKey, const std::string &strItem, const std::string &strFunName);
    REDISStatus ZaddPrivate(const std::string &strKey, int64_t score, const  std::string &strItem, const std::string &strFunName);
    REDISStatus ZSremPrivate(const std::string &strKey, const std::string &strItem, const std::string &strFunName);
    REDISStatus ZScorePrivate(const std::string &strKey, const  std::string &strItem, int64_t& nRet, const std::string &strFunName);
	REDISStatus ZCountPrivate(const std::string &strKey, int64_t from, const char *to, int64_t& nCount, const std::string &strFunName);
    REDISStatus ZCardPrivate(const std::string &strKey, int64_t& nLen, const std::string &strFunName);

    REDISStatus ZRangeWithScorePrivate(const std::string &strKey, std::vector<std::string> &topicItem, int64_t from, int64_t to, std::vector<std::string> &scoreItem, const std::string &strFunName);
    REDISStatus ZRangeByScorePrivate(const std::string &strKey, int64_t from, const char *to, std::vector<std::string> &memberItem, const std::string &strFunName);

    REDISStatus ZRangeByScoreWithScoresPrivate(const std::string &strKey, int64_t from, const char *to, std::vector<std::string> &rgMemberItem, std::vector<std::string> &rgScoreItem, const std::string &strFunName);
   
    REDISStatus ZRangeByScoreAndLimitPrivate(const std::string &strKey, const std::string &strStart, const std::string &strEnd, int64_t nLimit, std::vector<std::string> &memberItem, const std::string &strFunName);
    REDISStatus ZRangeByScoreAndLimitWithScorePrivate(const std::string &strKey, const std::string &strStart, const std::string &strEnd, int64_t nLimit, std::vector<std::string> &memberItem, std::vector<std::string> &scoreItem, const std::string &strFunName);

    REDISStatus ZReverRangeByScoreAndLimitWithScoresPrivate(const std::string &strKey, int64_t nStart, int64_t nLimit, std::vector<std::string> &memberItem, std::vector<std::string> &scoreItem, const std::string &strFunName);
    REDISStatus ZReverRangeByScoreAndStartEndLimitWithScorePrivate(const std::string &strKey, const std::string &strStart, const std::string &strEnd, int64_t nLimit, std::vector<std::string> &memberItem, std::vector<std::string> &scoreItem, const std::string &strFunName);
    REDISStatus ZReverRangeByScoreAndLimitPrivate(const std::string &strKey, const std::string &strStart, const std::string &strEnd, int64_t nLimit, std::vector<std::string> &memberItem, const std::string &strFunName);

    REDISStatus ZSetGetPeerMsgsPrivate(const std::string &strKey, std::vector<std::string> &msgs, int32_t& nTotalSize, const std::string &strFunName);
    REDISStatus ZremByScorePrivate(const std::string &strKey, int64_t scorefrom, int64_t scoreto, const std::string &strFunName);
    REDISStatus ZSetIncrbyPrivate(const std::string &strKey, const std::string &strMember, const std::string &strScore, int64_t& bRet, const std::string &strFunName);

	//-----------------哈希操作----------------------------
    REDISStatus DelKeyPrivate(const std::string &strKeyName, const std::string &strFunName);
    REDISStatus HsetPrivate(const std::string &strKey, const std::string &strField, const std::string &strItem, const std::string &strFunName);
    REDISStatus HMsetPrivate(const std::string &strKey,const std::vector<std::pair<std::string,std::string> > &hashItemsPair,const std::string &strFunName);
    REDISStatus HMgetPrivate(const std::string &strKey,const std::vector<std::string> &hashItemFields,std::vector<std::string> &hashItemValues,const std::string &strFunName);
    REDISStatus Commandargv(const std::vector<std::string> &vData,const std::string &strFunName);
    REDISStatus CommandargvArray(const std::vector<std::string> &vData,std::vector<std::string> &vDataValues,const std::string &strFunName);
    REDISStatus HdelPrivate(const std::string &strKey, const std::string &strField, const std::string &strFunName);
    REDISStatus HgetPrivate(const std::string &strKey, const std::string &strField, std::string &strItem, const std::string &strFunName);
    REDISStatus HgetAllPrivate(const std::string &strKey, std::map<std::string, std::string> &mapFileValue, const std::string &strFunName);
    REDISStatus GetSeqIDPrivate(const std::string &strKey, int64_t& nRet, const std::string &strFunName);
    REDISStatus HIncrbyPrivate(const std::string &strKey, const std::string &strField, int64_t &unItem, int64_t val, const std::string &strFunName);
    REDISStatus HScanPrivate(const std::string &strKey, const std::string &strHashItemMatch, int count, int& cursor, std::list<std::string>& hashItems, std::list<std::string>& hashItemsValue, const std::string &strFunName);

    bool getRedisServer(std::string &address);
    bool ConnectDBPrivate(const std::string &strIp, int32_t nPort);

	redisContext *m_pRedisDBContex;
    
	std::string m_strIp;
	int32_t m_nPort;
};

#endif
