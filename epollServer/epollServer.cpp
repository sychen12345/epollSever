//
// Created by Lenovo on 2022/5/30.
//

#include "epollServer.h"
#include "connectPool.h"
#include "eventCallBack.h"
#include <iostream>
#include <cstring>
using std::endl;
using std::cout;
int epollServer::epollInit()
{
    mEpollFd = epoll_create(mWorkerConnections); // 最大链接
    if (mEpollFd == -1)
    {
        cout << "epollServer::epollInit() error";
        exit(2);
    }
    mConnectPool->initconnection(); // 初始化一个连接池

    auto pos = mListenSocketList.begin();
    for (; pos != mListenSocketList.end(); ++pos)
    {
        connectionPtr pConn = mConnectPool->getConnection((*pos)->fd); // 取出对象
        pConn->listening    = (*pos);                                  // 连接对象 和监听对象关联，方便通过连接对象找监听对象
        (*pos)->connection  = pConn;                                   // 监听对象 和连接对象关联，方便通过监听对象找连接对象

        pConn->readHandler = std::bind(&epollServer::eventAccept, this, std::placeholders::_1);
        // 绑定连接回调函数

        if (epollOperEvent((*pos)->fd, EPOLL_CTL_ADD, EPOLLIN | EPOLLRDHUP, 0, pConn) == -1)
        {
            exit(2); //有问题，直接退出，日志 已经写过了
        }
    } // end for
    return 1;
}
epollServer::epollServer() : mWorkerConnections(1024), mConnectPool(new connectPool()), mNetListener(new netListener())
{
    mEventCallBack    = new eventCallBack(mConnectPool);
    mListenSocketList = std::move(mNetListener->getListenSocketList());
}
int epollServer::epollAddEvent(int fd, int readevent, int writeevent, uint32_t otherflag, uint32_t eventtype, connectionPtr c)
{
    epoll_event ev;
    // int op;
    memset(&ev, 0, sizeof(ev));

    if (readevent == 1) { ev.events = EPOLLIN | EPOLLRDHUP; }
    if (otherflag != 0) { ev.events |= otherflag; }
    ev.data.ptr = reinterpret_cast<void *>(c->instance);
    // 抄自muduo Channel* channel = static_cast<Channel*>(events_[i].data.ptr);

    if (epoll_ctl(mEpollFd, eventtype, fd, &ev) == -1)
    {
        cout << "CSocekt::epollAddEvent()中epoll_ctl失败." << endl;

        return -1;
    }
    return 1;
}
int epollServer::epollOperEvent(int fd, uint32_t eventtype, uint32_t flag, int bcaction, connectionPtr pConn)
{
    epoll_event ev{};
    if (eventtype == EPOLL_CTL_ADD)
    {
        ev.events     = flag;
        pConn->events = flag; // 添加事件
    }
    else if (eventtype == EPOLL_CTL_MOD)
    {
        // 修改
        ev.events = pConn->events;
        if (bcaction == 0) { ev.events |= flag; }
        else if (bcaction == 1)
        {
            //去掉某个标记
            ev.events &= ~flag;
        }
        else
        {
            // 完全覆盖某个标记
            ev.events = flag;
        }
        pConn->events = ev.events; // 记录下该标记
    }
    else
    {
        return 1;
    }
    ev.data.ptr = (void *)pConn;
    if (epoll_ctl(mEpollFd, eventtype, fd, &ev) == -1)
    {
        cout << "CSocekt::epollOperEvent()中epoll_ctl(%d,%ud,%ud,%d)失败." << fd << " " << eventtype << " flag = " << flag << "  bcaction = " << bcaction
             << endl;
        return -1;
    }
    return 1;
}
void epollServer::eventAccept(connectionPtr pConn)
{
    struct sockaddr mysockaddr; // 远端服务器的socket地址
    socklen_t       socklen;
    int             err;
    int             s;
    static int      useAccept4 = 1;
    connectionPtr   newc;

    socklen = sizeof(mysockaddr);
    do {
        if (useAccept4)
        {
            // listen非阻塞的
            s = accept4(pConn->fd, &mysockaddr, &socklen, SOCK_NONBLOCK);
        }
        else
        {
            s = accept(pConn->fd, &mysockaddr, &socklen);
        }

        if (s == -1)
        {
            err = errno;

            if (err == EAGAIN) { return; }
            if (err == ECONNABORTED) // 连接过程中对面拔掉网线
            {
                cout << "忽略该错误，再次调用" << endl;
            }
            else if (err == EMFILE || err == ENFILE)
            {
                cout << "文件描述符达到最大数" << endl;
                return;
            }
            if (useAccept4 && err == ENOSYS)
            {
                useAccept4 = 0;
                continue;
            }
            if (err == ECONNABORTED)
            {
                // 对方关闭套接字
            }
            if (err == EMFILE || err == ENFILE) {}
            return;
        } // end if(s == -1)

        // 防止短时间内连接大量用户
        if (mConnectPool->mConnectionList.size() > (mWorkerConnections * 5))
        {
            if (mConnectPool->mFreeconnectionList.size() < mWorkerConnections)
            {
                close(s);
                return;
            }
        }
        newc = mConnectPool->getConnection(s);

        memcpy(&newc->sSockaddr, &mysockaddr, socklen);
        // 拷贝客户端地址到连接对象【要转成字符串ip地址参考函数ngx_sock_ntop()】

        if (!useAccept4)
        {
            //如果不是用accept4()取得的socket，那么就要设置为非阻塞【因为用accept4()的已经被accept4()设置为非阻塞了】
            if (!mNetListener->setNonBlocking(s)) // set 失败
            {
                mConnectPool->closeConnection(newc);
                return; //直接返回
            }
        }

        newc->listening = pConn->listening;
        // 连接对象 和监听对象关联，方便通过连接对象找监听对象【关联到监听端口】
        // 绑定两个事件的回调函数

        newc->readHandler  = std::bind(&eventCallBack::readRequestHandler, mEventCallBack, std::placeholders::_1);
        newc->writeHandler = std::bind(&eventCallBack::writeRequestHandler, mEventCallBack, std::placeholders::_1);

        if (epollOperEvent(s, EPOLL_CTL_ADD, EPOLLIN | EPOLLRDHUP, 0, newc) == -1)
        {
            mConnectPool->closeConnection(newc); // 回收
            return;
        }
        ++mConnectPool->mOnlineUserCount; // 连入数量+1
        break;
    } while (1);
}
void epollServer::loop()
{
    epollProcessEvents(-1);
}
int  epollServer::epollProcessEvents(int timer)
{
    int events = epoll_wait(mEpollFd, mEvents, MAX_EVENTS, timer);

    if (events == -1)
    {
        if (errno == EINTR)
        {
            // 来信号打断了
            return 1;
        }
        else
        {
            cout << "epollProcessEvents()中epoll_wait()失败!" << endl;
            return 0; //非正常返回
        }
    }
    if (events == 0)
    {
        if (timer != -1)
        {
            cout << "epollProcessEvents阻塞时间到" << endl;
            return 1;
        }
        cout << "epollProcessEvents()中epoll_wait()没超时却没返回任何事件!" << endl;
        return 1;
    }
    connectionPtr pConn;
    uint32_t      revents; // 事件类型
    for (int i = 0; i < events; ++i)
    // 遍历本次epoll_wait返回的所有事件
    {
        pConn   = (connectionPtr)(mEvents[i].data.ptr);
        revents = mEvents[i].events; // 事件类型

        if (revents & EPOLLIN) //如果是读事件
        {
            cout << "新连接加入" << endl;
            pConn->readHandler(pConn);
        }

        if (revents & EPOLLOUT)
        // 写事件
        {
            if (revents & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            // 客户端关闭
            {
                --pConn->iThrowsendCount;
            }
            else
            {
                pConn->writeHandler(pConn);
            }
        }
    }
    return 0;
}
