//
// Created by Lenovo on 2022/5/30.
//

#ifndef EPOLLTCP_EPOLLSERVER_H
#define EPOLLTCP_EPOLLSERVER_H

#include <atomic>
#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include "netListener.h"
#include "connectPtr.h"
class connectPool; // 一个连接池
class eventCallBack;
class epollServer
{
public:
    friend class epollServer;
    epollServer();
    int  epollInit();
    void loop();

private:
    int                       epollProcessEvents(int timer);
    epoll_event               mEvents[MAX_EVENTS]{};
    int                       mEpollFd;
    int                       mWorkerConnections; // 最大连接数
    connectPool              *mConnectPool;       // 维护连接池指针
    std::vector<listeningPtr> mListenSocketList;  // 监听套接字队列
    netListener              *mNetListener;       // 套接字对象
    int                       epollOperEvent(int fd, uint32_t eventtype, uint32_t flag, int bcaction, connectionPtr pConn);

    int epollAddEvent(int fd, int readevent, int writeevent, uint32_t otherflag, uint32_t eventtype, connectionPtr c);

private:
    void eventAccept(connectionPtr pConn); // 建立新连接
protected:
    eventCallBack *mEventCallBack; // 回调类
};


#endif // EPOLLTCP_EPOLLSERVER_H
