//
// Created by Lenovo on 2022/5/30.
//

#ifndef EPOLLTCP_CONNECTPOOL_H
#define EPOLLTCP_CONNECTPOOL_H


#include <list>
#include <semaphore.h>
#include <unordered_map>
#include <map>
#include "connectPtr.h"
class connectPool
{
public:
    friend class epollServer;
    connectPool();
    ~connectPool();
    void          initconnection();                        // 初始化连接池
    void          closeConnection(connectionPtr pConn);    // 关闭连接
    void          clearconnection();                       // 回收连接池
    void          freeConnection(connectionPtr pConn);     // 释放连接到连接池中
    void          inRecyConnectQueue(connectionPtr pConn); // 回收队列
    void          onlineAddMap(connectionPtr pConn);       // 将在线连接添加到池中
    connectionPtr getConnection(int isock);                // 获取一个连接
private:
    std::list<connectionPtr> mConnectionList; // 连接列表【连接池】
    std::list<connectionPtr> mFreeconnectionList; // 空闲连接列表【这里边装的全是空闲的连接】
    std::list<connectionPtr> mRecyconnectionList; // 将要释放的连接放这里

    std::unordered_map<std::string, connectionPtr> mOnlineConnectMap;    // 在线的连接池
    std::atomic<int>                               mOnlineConnectionN{}; // 在线的连接数量
    std::atomic<int>                               mTotalConnectionN{};  // 连接池总连接数
    std::atomic<int>                               mFreeConnectionN{};   // 连接池空闲连接数


    pthread_mutex_t  mConnectionMutex{};       // 连接相关互斥量
    pthread_mutex_t  mOnlineConnectionMutex{}; // 在线连接相关互斥量
    pthread_mutex_t  mRecyconnqueueMutex{};    // 连接回收队列相关的互斥量
    std::atomic<int> mTotolRecyconnectionN;    // 待释放连接队列大小
    int              mRecyConnectionWaitTime;  // 等待这么些秒后才回收连接
    int              mWorkerConnections;       // 创建多少个连接，从配置文件获取
    /**
     * 在线用户相关变量
     */
    std::atomic<int>                               mOnlineUserCount; // 当前在线用户数统计
    std::unordered_map<std::string, commPkgHeader> mUser;

private:
    /**
     * 消息队列相关变量
     */
    std::list<char *> mMsgSendQueue;       // 发送数据消息队列
    std::atomic<int>  mISendMsgQueueCount; // 发消息队列大小
    // 多线程相关
    pthread_mutex_t mSendMessageQueueMutex{}; // 发消息队列互斥量
    sem_t           mSemEventSendQueue{};     // 处理发消息线程相关的信号量
};


#endif // EPOLLTCP_CONNECTPOOL_H
