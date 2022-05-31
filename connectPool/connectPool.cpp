//
// Created by Lenovo on 2022/5/30.
//

#include "connectPool.h"
#include "cLock.h"
#include "cMemory.h"
#include <iostream>
#include <cstring>
using std::endl;
using std::cout;
connectPool::connectPool() {}

connectPool::~connectPool() {}

void connectPool::initconnection()
{
    connectionPtr pConn;
    cMemory      *pMemory      = cMemory::getInstance();
    int           ilenconnpool = sizeof(connectStruct);
    for (int i = 0; i < mWorkerConnections; ++i) //先创建这么多个连接，后续不够再增加
    {
        pConn = (connectionPtr)pMemory->allocMemory(ilenconnpool, true);

        pConn = new (pConn) connectStruct(); // 定位new指定空间
        pConn->getOneToUse();
        mConnectionList.push_back(pConn);     // 所有连接都放在这个list
        mFreeconnectionList.push_back(pConn); // 空闲连接会放在这个list
    }
    mFreeConnectionN = mTotalConnectionN = mConnectionList.size();
}

void connectPool::closeConnection(connectionPtr c)
{
    if (close(c->fd) == -1) { cout << "CSocekt::closeConnection()中close(%d)失败!" << endl; }
    c->fd = -1;
    memset(c->sessionId, 0, sizeof(c->sessionId)); // 将sessionId置空
    freeConnection(c);
}
void connectPool::clearconnection()
{
    connectionPtr pConn;
    cMemory      *pMemory = cMemory::getInstance();

    while (!mConnectionList.empty()) // 如果连接池队列不为空
    {
        pConn = mConnectionList.front(); // 获取头节点
        mConnectionList.pop_front();     // 将尾节点弹出
        pConn->~connectStruct();         // 手动调用析构
        pMemory->freeMemory(pConn);      // 释放内存
    }
}
void connectPool::freeConnection(connectionPtr pConn)
{
    {
        cLock lock(&mConnectionMutex); // 在作用域内加锁
        pConn->putOneToFree();
        pConn->loginFlag = false;
        mFreeconnectionList.push_back(pConn); // 空闲连接数+1 在线连接数-1
    }
    {
        cLock lock(&mOnlineConnectionMutex);
        if (pConn->strSession.size())
        {
            mOnlineConnectMap.erase(pConn->strSession); // 删除指定的会话连接
            std::cout << "删除指定的连接 " << pConn->strSession << " " << mOnlineConnectMap.size()
                      << std::endl;

            --mOnlineUserCount;
        }
    }

    ++mFreeConnectionN;
}
connectionPtr connectPool::getConnection(int isock)
{
    cLock lock(&mConnectionMutex);

    if (!mFreeconnectionList.empty())
    {
        // 有空闲的，自然是从空闲的中摘取
        connectionPtr pConn = mFreeconnectionList.front(); // 返回第一个元素但不检查元素存在与否
        mFreeconnectionList.pop_front();                   // 移除第一个元素但不返回
        pConn->getOneToUse();
        --mFreeConnectionN;
        pConn->fd = isock;
        return pConn;
    }

    // 走到这里，表示没空闲的连接了，那就考虑重新创建一个连接
    cMemory      *pMemory = cMemory::getInstance();
    connectionPtr pConn   = (connectionPtr)pMemory->allocMemory(sizeof(connectStruct), true);
    pConn = new (pConn) connectStruct(); // 定位new， 在指定的内存开辟空间
    pConn->getOneToUse();                // new出来需要初始化一些数据
    mConnectionList.push_back(pConn);    // 新建立的连接加入到池中
    ++mTotalConnectionN;
    pConn->fd = isock;
    return pConn;
}
void connectPool::inRecyConnectQueue(connectionPtr pConn) {}
void connectPool::onlineAddMap(connectionPtr pConn)
{
    cLock lock(&mOnlineConnectionMutex);
    auto  p = reinterpret_cast<char *>(pConn->sessionId);
    if (strlen(p))
    {
        // 关键时刻需要从该队列中查找
        std::string strSessionId;
        for (int j = 0; j < 16; ++j)
        {
            char str1[2] = { 0 };
            sprintf(str1, "%x", pConn->unsignedId[j]);
            strSessionId += str1;
        }
        if (mOnlineConnectMap.count(strSessionId)) { return; }
        pConn->strSession               = strSessionId;
        mOnlineConnectMap[strSessionId] = pConn;
        cout << "加入到在线连接池中  " << strSessionId << endl;
    }
}
