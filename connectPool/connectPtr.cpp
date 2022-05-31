//
// Created by Lenovo on 2022/5/30.
//
#include "cMemory.h"
#include "connectPtr.h"

connectStruct::connectStruct() : iCurrsequence(0) {}

connectStruct::~connectStruct() = default;

void connectStruct::getOneToUse()
{
    ++iCurrsequence;

    fd      = -1;          // 开始先给-1
    curStat = PKG_HD_INIT; // 收包状态处于 初始状态，准备接收数据包头【状态机】
    precvbuf = dataHeadInfo;
    // 收包我要先收到这里来，因为我要先收包头，所以收数据的buff直接就是dataHeadInfo
    irecvlen        = sizeof(commPkgHeader); // 这里指定收数据的长度，这里先要求收包头这么长字节的数据
    precvMemPointer = nullptr; // 既然没new内存，那自然指向的内存地址先给NULL
    iThrowsendCount = 0;       // 原子的
    psendMemPointer = nullptr; // 发送数据头指针记录
    events          = 0;       // epoll事件先给0
    lastPingTime    = time(nullptr); // 上次ping的时间

    floodkickLastTime = 0; // Flood攻击上次收到包的时间
    floodAttackCount  = 0; // Flood攻击在该时间内收到包的次数统计
    iSendCount        = 0;

    // 发送队列中有的数据条目数，若client只发不收，则可能造成此数过大，依据此数做出踢出处理
}

void connectStruct::putOneToFree()
{
    ++iCurrsequence;
    if (precvMemPointer != nullptr)
    {
        cMemory::getInstance()->freeMemory(precvMemPointer);
        precvMemPointer = nullptr;
    }
    if (psendMemPointer != nullptr)
    {
        cMemory::getInstance()->freeMemory(psendMemPointer);
        psendMemPointer = nullptr;
    }

    iThrowsendCount = 0; // 设置不设置感觉都行
}
