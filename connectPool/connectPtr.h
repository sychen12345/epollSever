//
// Created by Lenovo on 2022/5/30.
//

#ifndef EPOLLTCP_CONNECTPTR_H
#define EPOLLTCP_CONNECTPTR_H

#include <unistd.h>
#include <pthread.h>
#include <functional>
#include <atomic>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "comm.h"
#include "netListener.h"

typedef struct connectStruct   connectStruct, *connectionPtr;
typedef struct listeningStruct listeningStruct, *listeningPtr;
struct listeningStruct // 和监听端口有关的结构
{
    int           port;       // 监听的端口号
    int           fd;         // 套接字句柄socket
    connectionPtr connection; // 连接池中的一个连接，注意这是个指针
};
struct connectStruct
{
    connectStruct();

    virtual ~connectStruct();
    unsigned     instance : 1;   // 一个位域
    void         getOneToUse();  // 初始化
    void         putOneToFree(); // 连接被回收
    int          fd{};           // 文件描述符socket
    listeningPtr listening{};    // 监听队列
    int          fileFd{};       // 如果这个连接被赋予了传输文件的任务
    struct stat  fileStat
    {
    };
    unsigned char sessionId[16]  = { 0 };
    unsigned char unsignedId[16] = { 0 };
    std::string   strSession;
    bool          loginFlag = false;

    uint64_t iCurrsequence;
    sockaddr sSockaddr{}; // 保存对方地址信息用的

    uint8_t wReady{};

    /**
     * 和epoll事件有关 ptr callback events
     */
    using readCallback  = std::function<void(connectionPtr)>;
    using writeCallback = std::function<void(connectionPtr)>;
    readCallback  readHandler;
    writeCallback writeHandler;

    uint32_t events{}; // 和epoll事件有关
    /**
     * 和收包状态以及缓冲区有关
     */
    unsigned char curStat;                       // 当前收包的状态
    char          dataHeadInfo[PKG_HEADER_SIZE]; // 用于保存收到的数据的包头信息
    char         *precvbuf;                      // 接收数据的缓冲区的头指针
    unsigned int  irecvlen;                      // 收数据包头的长度
    char         *precvMemPointer;               // 标志接收缓冲区的头指针

    pthread_mutex_t  logicPorcMutex{};  // 逻辑处理相关的互斥量
    std::atomic<int> iThrowsendCount;   // 发送消息标记是否满
    char            *psendMemPointer{}; // 释放指针
    char            *psendbuf{};        // 发送缓冲区指针
    unsigned int     isendlen{};        // 要发送多少数据


    time_t inRecyTime{};   // 入到资源回收站里去的时间
    time_t lastPingTime{}; //上次ping的时间

    /**
     * 和网络安全相关
     */
    uint64_t         floodkickLastTime{}; // Flood攻击上次收到包的时间
    int              floodAttackCount{};  // Flood攻击在该时间内收到包的次数统计
    std::atomic<int> iSendCount{};        // 发送队列中有的数据条目数
    int              mIWaitTime{};        // 等待时间
    connectionPtr    next{};              // 用于连接池
};


#endif // EPOLLTCP_CONNECTPTR_H
