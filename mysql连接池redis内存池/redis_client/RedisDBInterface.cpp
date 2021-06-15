/*
 * @Author: 零声学院 Darren老师 QQ:326873713
 * @Date: 2019-12-09 17:31:53
 * @LastEditTime: 2020-05-27 20:21:44
 * @LastEditors: Please set LastEditors
 * @Description: Redis客户端接口
 * @FilePath: RedisDBInterface.cpp
 */
#include "RedisDBInterface.h"
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <assert.h>
#include <stdexcept>

#define MAX_RECONNEC_TIMES 3

#define REDIS_REPLY_STRING_TYPE 0x01
#define REDIS_REPLY_ARRAY_TYPE 0x02
#define REDIS_REPLY_INTEGER_TYPE 0x04
#define REDIS_REPLY_NIL_TYPE 0x08
#define REDIS_REPLY_STATUS_TYPE 0x10
#define REDIS_REPLY_ERROR_TYPE 0x20

const int nREDISDB_TRANSFER_SECONDS = 500;

#define LOG_ERROR(msg, ...)     \
	printf(msg, ##__VA_ARGS__); \
	printf("\n")
#define LOG_WARN(msg, ...)      \
	printf(msg, ##__VA_ARGS__); \
	printf("\n")
#define LOG_INFO(msg, ...)      \
	printf(msg, ##__VA_ARGS__); \
	printf("\n")
#define LOG_DEBUG(msg, ...)     \
	printf(msg, ##__VA_ARGS__); \
	printf("\n")
/*------------------------------------- GLOBAL DATA -----------------------------------*/
/*----------------------------- LOCAL FUNCTION PROTOTYPES -----------------------------*/
/*-------------------------------- FUNCTION DEFINITION --------------------------------*/

#define FORMAT_STREAM(__STROUPUT__, __INPUT__)                                                          \
	{                                                                                                   \
		try                                                                                             \
		{                                                                                               \
			std::ostringstream strSourceStream;                                                         \
			strSourceStream << __INPUT__;                                                               \
			__STROUPUT__ = strSourceStream.str();                                                       \
		}                                                                                               \
		catch (std::exception * e)                                                                      \
		{                                                                                               \
			LOG_ERROR("FORMAT_STREAM ERROR, file:%s,line:%d,reason:%s", __FILE__, __LINE__, e->what()); \
		}                                                                                               \
	}

#define DO_REDIS_PUB_FUN(__FUN__, __FUNERROR_NAME__)                        \
	;                                                                       \
	{                                                                       \
		int nCount = 0;                                                     \
		REDISStatus emRest = REDISStatus::KVDB_ERROR;                    \
		while (true)                                                        \
		{                                                                   \
			if (!IsConnected())                                             \
			{                                                               \
				emRest = REDISStatus::KVDB_ERROR;                        \
			}                                                               \
			else                                                            \
			{                                                               \
				emRest = __FUN__;                                           \
			}                                                               \
			if ((REDISStatus::KVDB_ERROR == emRest) && !IsConnected())   \
			{                                                               \
				if (nCount < MAX_RECONNEC_TIMES)                            \
				{                                                           \
					if (!ReConnectDB())                                     \
					{                                                       \
						struct timeval now;                                 \
						gettimeofday(&now, NULL);                           \
						uint32_t seed = now.tv_sec * 1000000 + now.tv_usec; \
						srandom(seed);                                      \
						usleep(random() % 700001);                          \
					}                                                       \
					nCount++;                                               \
				}                                                           \
				else                                                        \
				{                                                           \
					break;                                                  \
				}                                                           \
			}                                                               \
			else                                                            \
			{                                                               \
				break;                                                      \
			}                                                               \
		}                                                                   \
		return emRest;                                                      \
	}

static inline std::string tostr(uint32_t user_id)
{
	std::stringstream ss;
	ss << user_id;
	return ss.str();
}

CRedisDBInterface::~CRedisDBInterface()
{
	if (NULL != m_pRedisDBContex)
	{
		redisFree(m_pRedisDBContex);
		m_pRedisDBContex = NULL;
	}
}

bool CRedisDBInterface::ConnectDB()
{
	return ConnectDBPrivate(m_strIp, m_nPort);
}
bool CRedisDBInterface::ConnectDB(const std::string &strIp, int32_t nPort)
{
	m_strIp = strIp;
	m_nPort = nPort;
	return ConnectDBPrivate(m_strIp, m_nPort);
}

bool CRedisDBInterface::ReConnectDB()
{
	return ConnectDBPrivate(m_strIp, m_nPort);
}
REDISStatus CRedisDBInterface::FlushDB()
{
	std::string strFunName = "FLUSHDB";
	DO_REDIS_PUB_FUN(FlushDBPrivate(strFunName, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::DelKey(const std::string &strKey)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "DEL" << strKey);
	DO_REDIS_PUB_FUN(DelKeyPrivate(strKey, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::IncrSeq(const std::string &strName, uint64_t &unItem)
{
	std::string strFunName = "INCR " + strName;
	DO_REDIS_PUB_FUN(IncrSeqPrivate(strName, unItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Keys(const std::string &pattern, std::list<std::string> &keyItems)
{
	std::string strFunName = "KEYS " + pattern;
	DO_REDIS_PUB_FUN(KeysPrivate(pattern, keyItems, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ListRPop(const std::string &strName, std::string &strItem)
{
	std::string strFunName = "RPOP " + strName;
	DO_REDIS_PUB_FUN(ListRPopPrivate(strName, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ListLPop(const std::string &strName, std::string &strItem)
{
	std::string strFunName = "LPOP " + strName;
	DO_REDIS_PUB_FUN(ListLPopPrivate(strName, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ListLPush(const std::string &strName, const std::string &strItem)
{
	std::string strFunName = "LPUSH " + strName;
	DO_REDIS_PUB_FUN(ListLPushPrivate(strName, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ListRPush(const std::string &strName, const std::string &strItem)
{
	std::string strFunName = "RPUSH " + strName;
	DO_REDIS_PUB_FUN(ListRPushPrivate(strName, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ListSize(const std::string &strName, int64_t &nRet)
{
	std::string strFunName = "LLEN " + strName;
	DO_REDIS_PUB_FUN(ListSizePrivate(strName, nRet, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::IsStringKeyExits(const std::string &strKey, bool &bIsExit)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "EXISTS " << strKey);
	DO_REDIS_PUB_FUN(IsKeyExits(strKey, bIsExit, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Set(const std::string &strKey, const std::string &strVal)
{
	std::string strFunName = "SET " + strKey;
	DO_REDIS_PUB_FUN(SetPrivate(strKey, strVal, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Get(const std::string &strKey, std::string &strVal)
{
	std::string strFunName = "GET " + strKey;
	DO_REDIS_PUB_FUN(GetPrivate(strKey, strVal, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Scard(const std::string &strName, int64_t &nRet)
{
	std::string strFunName = "SCARD " + strName;
	DO_REDIS_PUB_FUN(ScardPrivate(strName, nRet, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Smembers(const std::string &strName, std::set<std::string> &rgItem)
{
	std::string strFunName = "SMEMBERS " + strName;
	DO_REDIS_PUB_FUN(SmembersPrivate(strName, rgItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Sismember(const std::string &strName, const std::string &strItem, bool &bIsExit)
{
	std::string strFunName = "SISMEMBER " + strName + " " + strItem;
	DO_REDIS_PUB_FUN(SismemberPrivate(strName, strItem, bIsExit, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Sadd(const std::string &strKey, const std::string &strItem)
{
	std::string strFunName = "SADD " + strKey;
	DO_REDIS_PUB_FUN(SaddPrivate(strKey, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Zadd(const std::string &strKey, int64_t score, const std::string &strItem)
{
	std::string strFunName = "ZADD " + strKey;
	DO_REDIS_PUB_FUN(ZaddPrivate(strKey, score, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Srem(const std::string &strKey, const std::string &strItem)
{
	std::string strFunName = "SREM " + strKey;
	DO_REDIS_PUB_FUN(SremPrivate(strKey, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Zrem(const std::string &strKey, const std::string &strItem)
{
	std::string strFunName = "ZREM " + strKey;
	DO_REDIS_PUB_FUN(ZSremPrivate(strKey, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZScore(const std::string &strKey, const std::string &strItem, int64_t &nRet)
{
	std::string strFunName = "ZSCORE " + strKey;
	DO_REDIS_PUB_FUN(ZScorePrivate(strKey, strItem, nRet, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZCount(const std::string &strKey, int64_t from, const char *to, int64_t &nRet)
{
	std::string strFunName = "ZCOUNT" + strKey;
	DO_REDIS_PUB_FUN(ZCountPrivate(strKey, from, to, nRet, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZCard(const std::string &strKey, int64_t &nLen)
{
	std::string strFunName = "ZCARD " + strKey;
	DO_REDIS_PUB_FUN(ZCardPrivate(strKey, nLen, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZRangeWithScore(const std::string &strKey, std::vector<std::string> &topicItem,
 	int64_t from, int64_t to, std::vector<std::string> &scoreItem)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "ZRANGE " << strKey << " " << from << " " << to);
	DO_REDIS_PUB_FUN(ZRangeWithScorePrivate(strKey, topicItem, from, to, scoreItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZRangeByScore(const std::string &strKey, int64_t from, const char *to, std::vector<std::string> &memberItem)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "ZRANGEBYSCORE " << strKey << " " << from << " " << to);
	DO_REDIS_PUB_FUN(ZRangeByScorePrivate(strKey, from, to, memberItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZRangeByScoreWithScores(const std::string &strKey, int64_t from, const char *to, std::vector<std::string> &memberItem,
													   std::vector<std::string> &scoreItem)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "ZRANGEBYSCORE " << strKey << " " << from << " " << to);
	DO_REDIS_PUB_FUN(ZRangeByScoreWithScoresPrivate(strKey, from, to, memberItem, scoreItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZRangeByScoreAndLimitWithScore(const std::string &strKey, int64_t nStart, int64_t nEnd, int64_t nLimit, std::vector<std::string> &memberItem, std::vector<std::string> &scoreItem)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "ZRANGEBYSCOREANDLIMITWITHSCORE " << strKey << " " << nStart << " " << nEnd << " " << nLimit);
	DO_REDIS_PUB_FUN(ZRangeByScoreAndLimitWithScorePrivate(strKey, tostr(nStart), tostr(nEnd), nLimit, memberItem, scoreItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZReverRangeByScoreAndLimitWithScores(const std::string &strKey, int64_t nStart, int64_t nLimit, std::vector<std::string> &memberItem, std::vector<std::string> &scoreItem)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "ZREVERRANGEBYSCOREANDLIMITWITHSCORES " << strKey << " " << nStart << " " << nLimit);
	DO_REDIS_PUB_FUN(ZReverRangeByScoreAndLimitWithScoresPrivate(strKey, nStart, nLimit, memberItem, scoreItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZReverRangeByScoreAndLimit(const std::string &strKey, int64_t nStart, int64_t nLimit, std::vector<std::string> &memberItem)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "ZREVERRANGEBYSCOREANDLIMIT" << strKey << " " << nStart << " " << nLimit);
	DO_REDIS_PUB_FUN(ZReverRangeByScoreAndLimitPrivate(strKey, tostr(nStart), MIN_SCORE, nLimit, memberItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZSetGetPeerMsgs(const std::string &strKey, std::vector<std::string> &msgs, int32_t &nTotalSize)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "ZRANGE " << strKey << " 0 -1 WITHSCORES");
	DO_REDIS_PUB_FUN(ZSetGetPeerMsgsPrivate(strKey, msgs, nTotalSize, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZremByScore(const std::string &strKey, int64_t scorefrom, int64_t scoreto)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "ZREMRANGEBYSCORE " << strKey << " " << scorefrom << " " << scorefrom);
	DO_REDIS_PUB_FUN(ZremByScorePrivate(strKey, scorefrom, scoreto, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ZSetIncrby(const std::string &strKey, const std::string &strMember, const std::string &strScore, int64_t &nRet)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "ZINCRBY " << strKey << " " << strScore);
	DO_REDIS_PUB_FUN(ZSetIncrbyPrivate(strKey, strScore, strMember, nRet, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Hset(const std::string &strKey, const std::string &strField, const std::string &strItem)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "HSET " << strKey << " " << strField);
	DO_REDIS_PUB_FUN(HsetPrivate(strKey, strField, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::HMset(const std::string &strKey,
									 const std::vector<std::pair<std::string, std::string>> &hashItemsPair)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "HMSET " << strKey << "");
	DO_REDIS_PUB_FUN(HMsetPrivate(strKey, hashItemsPair, strFunName), strFunName);
}
REDISStatus CRedisDBInterface::HMget(const std::string &strKey, const std::vector<std::string> &hashItemFields, std::vector<std::string> &hashItemValues)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "HMGET " << strKey << "");
	DO_REDIS_PUB_FUN(HMgetPrivate(strKey, hashItemFields, hashItemValues, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::HIncrby(const std::string &strKey, const std::string &strField, int64_t &unItem, int64_t val)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "HINCRBY " << strKey << " " << strField);
	DO_REDIS_PUB_FUN(HIncrbyPrivate(strKey, strField, unItem, val, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::HScan(const std::string &strKey, const std::string &strHashItemMatch, int count, int &cursor, std::list<std::string> &hashItems, std::list<std::string> &hashItemsValue)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "HSCAN " << strKey << " " << cursor << " MATCH " << strHashItemMatch << " COUNT " << count);
	DO_REDIS_PUB_FUN(HScanPrivate(strKey, strHashItemMatch, count, cursor, hashItems, hashItemsValue, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Hdel(const std::string &strKey, const std::string &strField)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "HDEL " << strKey << " " << strField);
	DO_REDIS_PUB_FUN(HdelPrivate(strKey, strField, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Hget(const std::string &strKey, const std::string &strField, std::string &strItem)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "HGET  " << strKey << " " << strField);
	DO_REDIS_PUB_FUN(HgetPrivate(strKey, strField, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::HgetAll(const std::string &strKey, std::map<std::string, std::string> &mapFileValue)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "HGETALL  " << strKey);
	DO_REDIS_PUB_FUN(HgetAllPrivate(strKey, mapFileValue, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::GetSeqID(const std::string &strKey, int64_t &nRet)
{
	std::string strFunName = "INCR " + strKey;
	DO_REDIS_PUB_FUN(GetSeqIDPrivate(strKey, nRet, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::ListIndexElement(const std::string &strName, int32_t nIndex, std::string &strItem)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "LINDEX " << strName << " " << nIndex);
	DO_REDIS_PUB_FUN(ListIndexElementPrivate(strName, nIndex, strItem, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Expire(const std::string &strName, uint64_t nNum)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "EXPIRE " << strName << " " << nNum);
	DO_REDIS_PUB_FUN(ExpirePrivate(strName, nNum, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::IncrSeqBy(const std::string &strName, uint64_t &unItem, uint64_t nNum)
{
	std::string strFunName;
	FORMAT_STREAM(strFunName, "INCRBY " << strName << nNum);
	DO_REDIS_PUB_FUN(IncrSeqByPrivate(strName, unItem, nNum, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::SetEX(const std::string &strKey, const std::string &strVal, uint64_t nNum)
{
	std::string strFunName; FORMAT_STREAM(strFunName, "SETEX " << strKey << " " << strVal << " " << nNum);
	DO_REDIS_PUB_FUN(SetEXPrivate(strKey, strVal, nNum, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Mset(const std::vector<std::string> &strKeys, 
	const std::vector<std::string> &strVals)
{
	std::string strFunName; FORMAT_STREAM(strFunName, "MSET");
	DO_REDIS_PUB_FUN(MsetPrivate(strKeys, strVals, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::Mget(const std::vector<std::string> &strKeys, 
	std::vector<std::string> &strVals)
{
	std::string strFunName; FORMAT_STREAM(strFunName, "MGET");
	DO_REDIS_PUB_FUN(MgetPrivate(strKeys, strVals, strFunName), strFunName);
}

REDISStatus CRedisDBInterface::IncrSeqPrivate(const std::string &strName, uint64_t &unItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "INCR %s", strName.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unItem = pReply->integer;
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::KeysPrivate(const std::string &pattern, std::list<std::string> &keyItems, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "KEYS %s", pattern.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		keyItems.clear();
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			std::string strKey;
			strKey.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			keyItems.push_front(strKey);
		}
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ListRPopPrivate(const std::string &strName, std::string &strItem, const std::string &strFunName)
{

	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "RPOP %s", strName.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		strItem.append(pReply->str, pReply->len);
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ListLPopPrivate(const std::string &strName, std::string &strItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "LPOP %s", strName.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		strItem.append(pReply->str, pReply->len);
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ListLPushPrivate(const std::string &strName, const std::string &strItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "LPUSH  %s %b", strName.c_str(), strItem.c_str(), strItem.size());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ListRPushPrivate(const std::string &strName, const std::string &strItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "RPUSH %s %b", strName.c_str(), strItem.c_str(), strItem.size());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ListSizePrivate(const std::string &strName, int64_t &nRet, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "LLEN %s", strName.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		nRet = pReply->integer;
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::GetSeqIDPrivate(const std::string &strKey, int64_t &nRet, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "INCR %s", strKey.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		nRet = pReply->integer;
	}
	freeReplyObject(pReply);

	return emRet;
}

REDISStatus CRedisDBInterface::SetPrivate(const std::string &strKey, const std::string &strVal, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "SET %s %b", strKey.c_str(), strVal.c_str(), strVal.size());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::GetPrivate(const std::string &strKey, std::string &strVal, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "GET %s", strKey.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		strVal.append(pReply->str, pReply->len);
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ScardPrivate(const std::string &strName, int64_t &nRet, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "SCARD %s", strName.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		nRet = pReply->integer;
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::SmembersPrivate(const std::string &strName, std::set<std::string> &rgItem, const std::string &strFunName)
{
	rgItem.clear();
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "SMEMBERS %s", strName.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		for (unsigned nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			redisReply *pReplyTmp = *(pReply->element + nIndex);
			std::string strTmp;
			strTmp.append(pReplyTmp->str, pReplyTmp->len);
			rgItem.insert(strTmp);
		}
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::SismemberPrivate(const std::string &strName, const std::string &strItem, bool &bIsExit, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "SISMEMBER  %s %s", strName.c_str(), strItem.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		if (0 < pReply->integer)
		{
			bIsExit = true;
		}
		else
		{
			bIsExit = false;
		}
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::SaddPrivate(const std::string &strKey, const std::string &strItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "SADD %s %b", strKey.c_str(), strItem.c_str(), strItem.size());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZaddPrivate(const std::string &strKey, int64_t score, const std::string &strItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZADD %s %lld %b", strKey.c_str(), score, strItem.c_str(), strItem.size());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::SremPrivate(const std::string &strKey, const std::string &strItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "SREM %s %b", strKey.c_str(), strItem.c_str(), strItem.size());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZSremPrivate(const std::string &strKey, const std::string &strItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZREM %s %b", strKey.c_str(), strItem.c_str(), strItem.size());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZScorePrivate(const std::string &strKey, const std::string &strItem, int64_t &nRet, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZSCORE %s %s", strKey.c_str(), strItem.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		nRet = atoll(pReply->str);
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZCountPrivate(const std::string &strKey, int64_t from, const char *to, int64_t &nRet, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZCOUNT %s %lld %s", strKey.c_str(), from, to);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		nRet = pReply->integer;
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZCardPrivate(const std::string &strKey, int64_t &nLen, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZCARD %s", strKey.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		nLen = pReply->integer;
	}
	freeReplyObject(pReply);
	return emRet;
}

/*
* from = 0, to = -1 :从第一个到最后一个
* 
*/
REDISStatus CRedisDBInterface::ZRangeWithScorePrivate(const std::string &strKey,
													  std::vector<std::string> &rgTopicItem, int64_t from, int64_t to, std::vector<std::string> &rgScoreItem,
													  const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZRANGE %s %lld %lld WITHSCORES", strKey.c_str(), from, to);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		rgTopicItem.clear();
		rgScoreItem.clear();
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			std::string strele;
			strele.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			rgTopicItem.push_back(strele);
			nIndex++;
			rgScoreItem.push_back(std::string(pReply->element[nIndex]->str));
		}
	}
	freeReplyObject(pReply);

	return emRet;
}

/*
ZRANGEBYSCORE 
* from: from score
* to  : to score
*/
REDISStatus CRedisDBInterface::ZRangeByScorePrivate(const std::string &strKey, int64_t from, const char *to, std::vector<std::string> &rgMemberItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZRANGEBYSCORE %s %lld %s", strKey.c_str(), from, to);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		rgMemberItem.clear();
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			std::string strele;
			strele.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			rgMemberItem.push_back(strele);
		}
	}

	freeReplyObject(pReply);
	return emRet;
}

/*
ZRANGEBYSCOREWITHSCORES 
* from: from score
* to  : to score
*/
REDISStatus CRedisDBInterface::ZRangeByScoreWithScoresPrivate(const std::string &strKey, int64_t from,
															  const char *to, std::vector<std::string> &rgMemberItem, std::vector<std::string> &rgScoreItem,
															  const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZRANGEBYSCORE %s %lld %s WITHSCORES", strKey.c_str(), from, to);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		rgMemberItem.clear();
		rgScoreItem.clear();
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			std::string strele;
			strele.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			rgMemberItem.push_back(strele);
			nIndex++;
			rgScoreItem.push_back(std::string(pReply->element[nIndex]->str));
		}
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZRangeByScoreAndLimitPrivate(const std::string &strKey, 
	const std::string &strStart, const std::string &strEnd, int64_t nLimit, 
	std::vector<std::string> &memberItem, const std::string &strFunName)
{
	memberItem.clear();
	std::string strCmd;
	FORMAT_STREAM(strCmd, "ZRANGEBYSCORE " << strKey << " " << strStart << " " << strEnd << " limit 0 " << nLimit);
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, strCmd.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			std::string strele;
			strele.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			memberItem.push_back(strele);
		}
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZRangeByScoreAndLimitWithScorePrivate(const std::string &strKey, 
	const std::string &strStart, const std::string &strEnd, int64_t nLimit, 
	std::vector<std::string> &memberItem, std::vector<std::string> &scoreItem, 
	const std::string &strFunName)
{
	memberItem.clear();
	scoreItem.clear();
	std::string strCmd;
	FORMAT_STREAM(strCmd, "ZRANGEBYSCORE " << strKey << " " << strStart << " " << strEnd << " limit 0 " << nLimit << " WITHSCORES");
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, strCmd.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			std::string strele;
			strele.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			memberItem.push_back(strele);
			nIndex++;
			scoreItem.push_back(std::string(pReply->element[nIndex]->str, pReply->element[nIndex]->len));
		}
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZReverRangeByScoreAndLimitWithScoresPrivate(const std::string &strKey, 
	int64_t nStart, int64_t nLimit, std::vector<std::string> &memberItem, 
	std::vector<std::string> &scoreItem, const std::string &strFunName)
{
	memberItem.clear();
	scoreItem.clear();
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZREVRANGEBYSCORE %s %lld -inf limit 0 %lld WITHSCORES", strKey.c_str(), nStart, nLimit);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			std::string strele;
			strele.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			memberItem.push_back(strele);
			nIndex++;
			scoreItem.push_back(std::string(pReply->element[nIndex]->str));
		}
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZReverRangeByScoreAndStartEndLimitWithScorePrivate(const std::string &strKey, 
	const std::string &strStart, const std::string &strEnd, int64_t nLimit, 
	std::vector<std::string> &memberItem, std::vector<std::string> &scoreItem, 
	const std::string &strFunName)
{
	memberItem.clear();
	scoreItem.clear();
	std::string strCmd;
	FORMAT_STREAM(strCmd, "ZREVRANGEBYSCORE " << strKey << " " << strStart << " " << strEnd << " limit 0 " << nLimit << " WITHSCORES");
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, strCmd.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			std::string strele;
			strele.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			memberItem.push_back(strele);
			nIndex++;
			scoreItem.push_back(std::string(pReply->element[nIndex]->str, pReply->element[nIndex]->len));
		}
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZReverRangeByScoreAndLimitPrivate(const std::string &strKey, 
	const std::string &strStart, const std::string &strEnd, int64_t nLimit, 
	std::vector<std::string> &memberItem, const std::string &strFunName)
{
	memberItem.clear();
	std::string strCmd;
	FORMAT_STREAM(strCmd, "ZREVRANGEBYSCORE " << strKey << " " << strStart << " " << strEnd << " limit 0 " << nLimit);
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, strCmd.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			std::string strele;
			strele.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			memberItem.push_back(strele);
		}
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZSetGetPeerMsgsPrivate(const std::string &strKey, std::vector<std::string> &msgs, int32_t &nTotalSize, const std::string &strFunName)
{
	nTotalSize = 0;
	msgs.clear();
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZRANGE %s 0 -1 WITHSCORES", strKey.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			msgs.push_back(std::string(pReply->element[nIndex]->str));
			nTotalSize += pReply->element[nIndex]->len;
			nIndex++;
		}
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZremByScorePrivate(const std::string &strKey, int64_t scorefrom, int64_t scoreto, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZREMRANGEBYSCORE  %s %lld %lld", strKey.c_str(), scorefrom, scoreto);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ZSetIncrbyPrivate(const std::string &strKey, const std::string &strMember, const std::string &strScore, int64_t &nRet, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "ZINCRBY %s %s %s", strKey.c_str(), strScore.c_str(), strMember.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		nRet = atoll(pReply->str);
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::CheckReply(const std::string &strFunName, redisReply *pReply)
{
	REDISStatus nRet = REDISStatus::KVDB_ERROR;
	if (NULL == pReply)
	{
		//连接被断开，如何进行进一步处理，需要知道
		redisFree(m_pRedisDBContex);
		m_pRedisDBContex = NULL;
		nRet = REDISStatus::KVDB_ERROR;
		return nRet;
	}

	if (REDIS_REPLY_ERROR == pReply->type)
	{
		LOG_ERROR("redisCommand exec '%s' failed, pReply->type = %d,pReply->str == %s", strFunName.c_str(),
				  pReply->type, pReply->str);
		nRet = REDISStatus::KVDB_ERROR;
	}
	else if (REDIS_REPLY_NIL == pReply->type)
	{
		LOG_DEBUG("redisCommand exec '%s' failed, pReply->type = %d, :REDIS_REPLY_NIL", strFunName.c_str(),
				  pReply->type);
		nRet = REDISStatus::KVDB_RNULL;
	}
	else
	{
		nRet = REDISStatus::KVDB_OK;
	}

	return nRet;
}

bool CRedisDBInterface::IsConnected()
{
	return NULL != m_pRedisDBContex;
}

REDISStatus CRedisDBInterface::FlushDBPrivate(const std::string &strKeyName, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "%s", strKeyName.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::HsetPrivate(const std::string &strKey, const std::string &strField, const std::string &strItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "HSET %s %s %b", strKey.c_str(), strField.c_str(), strItem.c_str(), strItem.size());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::Commandargv(const std::vector<std::string> &vData, const std::string &strFunName)
{
	std::vector<const char *> argv(vData.size());
	std::vector<size_t> argvlen(vData.size());
	unsigned int i = 0;
	for (std::vector<std::string>::const_iterator iter = vData.begin(); iter != vData.end(); ++iter, ++i)
	{
		argv[i] = iter->c_str(), argvlen[i] = iter->size();
	}
	//调用redis命令执行方法实现数组命令的执行
	redisReply *pReply = (redisReply *)redisCommandArgv(m_pRedisDBContex, argv.size(), &(argv[0]), &(argvlen[0]));
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

//实现hash 的多个filed的存储
REDISStatus CRedisDBInterface::HMsetPrivate(const std::string &strKey,
											const std::vector<std::pair<std::string, std::string>> &hashItemsPair,
											const std::string &strFunName)
{
	if (hashItemsPair.size() == 0)
	{
		LOG_WARN("redisCommand exec '%s' failed, reason: hashItemsPair.size==0, strKey:%s",
				 strFunName.c_str(), strKey.c_str());
		return REDISStatus::KVDB_ERROR;
	}

	std::vector<std::string> vCmdData;
	vCmdData.clear();
	vCmdData.push_back("HMSET");
	vCmdData.push_back(strKey);
	std::vector<std::pair<std::string, std::string>>::const_iterator iterPair = hashItemsPair.begin();
	for (; iterPair != hashItemsPair.end(); iterPair++)
	{
		vCmdData.push_back(iterPair->first);
		vCmdData.push_back(iterPair->second);
	}

	return Commandargv(vCmdData, strFunName);
}

REDISStatus CRedisDBInterface::HMgetPrivate(const std::string &strKey, const std::vector<std::string> &hashItemFields, std::vector<std::string> &hashItemValues, const std::string &strFunName)
{
	std::vector<std::string> vCmdData;
	vCmdData.clear();
	vCmdData.push_back("HMGET");
	vCmdData.push_back(strKey);
	std::vector<std::string>::const_iterator iter = hashItemFields.begin();
	for (; iter != hashItemFields.end(); iter++)
	{
		vCmdData.push_back(*iter);
	}
	return CommandargvArray(vCmdData, hashItemValues, strFunName);
}
REDISStatus CRedisDBInterface::CommandargvArray(const std::vector<std::string> &vData, std::vector<std::string> &vDataValues, const std::string &strFunName)
{
	std::vector<const char *> argv(vData.size());
	std::vector<size_t> argvlen(vData.size());
	unsigned int i = 0;
	for (std::vector<std::string>::const_iterator iter = vData.begin(); iter != vData.end(); ++iter, ++i)
	{
		argv[i] = iter->c_str(), argvlen[i] = iter->size();
	}
	redisReply *pReply = (redisReply *)redisCommandArgv(m_pRedisDBContex, argv.size(), &(argv[0]), &(argvlen[0]));
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			if(pReply->element[nIndex]->type == REDIS_REPLY_STRING)
				vDataValues.push_back(pReply->element[nIndex]->str);
			else	
				vDataValues.push_back("");
		}
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::HIncrbyPrivate(const std::string &strKey, const std::string &strField, int64_t &unItem, int64_t val, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "HINCRBY %s %s %lld", strKey.c_str(), strField.c_str(), val);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unItem = pReply->integer;
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::HScanPrivate(const std::string &strKey, const std::string &strHashItemMatch,
											int count, int &cursor, std::list<std::string> &hashItems,
											std::list<std::string> &hashItemsValue, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex,
													"HSCAN %s %u MATCH %s COUNT %lld", strKey.c_str(), cursor, strHashItemMatch.c_str(), count);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unsigned int nIndex = 0;
		cursor = atoi(pReply->element[0]->str);
		LOG_INFO("cursor:%d, elements:%ld", cursor, pReply->element[1]->elements);
		for (nIndex = 0; nIndex < pReply->element[1]->elements; nIndex++)
		{
			std::string strele;
			strele.append(pReply->element[1]->element[nIndex]->str, pReply->element[1]->element[nIndex]->len);
			hashItems.push_front(strele);
			nIndex++;
			hashItemsValue.push_front(std::string(pReply->element[1]->element[nIndex]->str));
		}
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::HdelPrivate(const std::string &strKey, const std::string &strField, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "HDEL %s %s", strKey.c_str(), strField.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::HgetPrivate(const std::string &strKey, const std::string &strField, std::string &strItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "HGET %s %s", strKey.c_str(), strField.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		strItem.append(pReply->str, pReply->len);
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::HgetAllPrivate(const std::string &strKey, std::map<std::string, std::string> &mapFileValue, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "HGETALL %s", strKey.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		mapFileValue.clear();
		unsigned int nIndex = 0;
		for (nIndex = 0; nIndex < pReply->elements; nIndex++)
		{
			std::string strField;
			strField.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			nIndex++;
			std::string strValue;
			strValue.append(pReply->element[nIndex]->str, pReply->element[nIndex]->len);
			mapFileValue.insert(std::pair<std::string, std::string>(strField, strValue));
		}
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::DelKeyPrivate(const std::string &strKeyName, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "DEL %s", strKeyName.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::IsKeyExits(const std::string &strKey, bool &bIsExit, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "EXISTS  %s", strKey.c_str());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		bIsExit = pReply->integer;
	}

	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ListIndexElementPrivate(const std::string &strName, int32_t nIndex, std::string &strItem, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "LINDEX  %s %d", strName.c_str(), nIndex);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		strItem.append(pReply->str, pReply->len);
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::IncrSeqByPrivate(const std::string &strName, uint64_t &unItem, uint64_t nNum, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "INCRBY %s %lld", strName.c_str(), nNum);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		unItem = pReply->integer;
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::ExpirePrivate(const std::string &strName, uint64_t nNum, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "EXPIRE %s %lld", strName.c_str(), nNum);
	REDISStatus emRet = CheckReply(strFunName, pReply);
	if (REDISStatus::KVDB_OK == emRet)
	{
		if (pReply->integer == 1)
		{
			emRet = REDISStatus::KVDB_OK;
		}
		else
		{
			emRet = REDISStatus::KVDB_RNULL;
		}
	}
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::SetEXPrivate(const std::string &strKey, const std::string &strVal, uint64_t nNum, const std::string &strFunName)
{
	redisReply *pReply = (redisReply *)redisCommand(m_pRedisDBContex, "SETEX %s %lld %b", strKey.c_str(), nNum, strVal.c_str(), strVal.size());
	REDISStatus emRet = CheckReply(strFunName, pReply);
	freeReplyObject(pReply);
	return emRet;
}

REDISStatus CRedisDBInterface::MsetPrivate(const std::vector<std::string> &strKeys, \
		const std::vector<std::string> &strVals, 									\
		const std::string &strFunName)
{
	if (strKeys.size() == 0 || strVals.size() == 0 || strKeys.size() != strVals.size())
	{
		LOG_WARN("redisCommand exec '%s' failed, reason: strKeys.size=%ld, strVals.size=%ld",
				 strFunName.c_str(), strKeys.size(), strVals.size());
		return REDISStatus::KVDB_ERROR;
	}

	std::vector<std::string> vCmdData;
	vCmdData.clear();
	vCmdData.push_back("MSET");
	auto itKey = strKeys.begin();
	auto itVals = strVals.begin();
	for (; itKey != strKeys.end(); itKey++, itVals++)
	{
		vCmdData.push_back(*itKey);
		vCmdData.push_back(*itVals);
	}

	return Commandargv(vCmdData, strFunName);
}

REDISStatus CRedisDBInterface::MgetPrivate(const std::vector<std::string> &strKeys, \
		std::vector<std::string> &strVals, 									\
		const std::string &strFunName)
{
	if (strKeys.size() == 0)
	{
		LOG_WARN("redisCommand exec '%s' failed, reason: strKeys.size==0", strFunName.c_str());
		return REDISStatus::KVDB_ERROR;
	}

	std::vector<std::string> vCmdData;
	vCmdData.clear();
	vCmdData.push_back("MGET");
	auto itKey = strKeys.begin();
	for (; itKey != strKeys.end(); itKey++)
	{
		vCmdData.push_back(*itKey);
	}

	return CommandargvArray(vCmdData, strVals, strFunName);
}

bool CRedisDBInterface::ConnectDBPrivate(const std::string &strIp, int32_t nPort)
{
	if (NULL != m_pRedisDBContex)
	{
		redisFree(m_pRedisDBContex);
		m_pRedisDBContex = NULL;
	}

	m_pRedisDBContex = redisConnect(strIp.c_str(), nPort);
	if (NULL == m_pRedisDBContex)
	{
		LOG_ERROR("CRedisDBInterface::ConnectDB failed strIp == %s, nPort == %d", strIp.c_str(), nPort);
		return false;
	}
	else
	{
		if (0 != m_pRedisDBContex->err)
		{
			LOG_ERROR("CRedisDBInterface::ConnectDB failed strIp == %s, nPort == %d, errorCode==%d",
					  strIp.c_str(),
					  nPort, m_pRedisDBContex->err);
			redisFree(m_pRedisDBContex);
			m_pRedisDBContex = NULL;
			return false;
		}
		LOG_INFO("CRedisDBInterface::ConnectDB success strIp == %s, nPort == %d", strIp.c_str(), nPort);
		return true;
	}
}