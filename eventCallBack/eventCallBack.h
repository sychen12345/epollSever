//
// Created by Lenovo on 2022/5/31.
//

#ifndef EPOLLTCP_EVENTCALLBACK_H
#define EPOLLTCP_EVENTCALLBACK_H
#include "connectPool.h"

class eventCallBack
{
public:
    std::atomic<int> mCount; // 测试记录用的


    eventCallBack(connectPool *pool);
    void readRequestHandler(connectionPtr pConn);  // 设置数据来时的读处理函数
    void writeRequestHandler(connectionPtr pConn); // 设置要发送数据的回调函数


    connectPool *mConnectPool;
    size_t       mILenPkgHeader; // sizeof(commPkgHeader);
    size_t       mILenMsgHeader; // sizeof(strucMsgHeader);
};


#endif // EPOLLTCP_EVENTCALLBACK_H
