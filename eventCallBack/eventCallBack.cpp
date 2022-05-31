//
// Created by Lenovo on 2022/5/31.
//

#include "eventCallBack.h"
#include <cMemory.h>

#include <arpa/inet.h>
#include <iostream>
using std::cout;
using std::endl;

eventCallBack::eventCallBack(connectPool *pool) : mConnectPool(pool) {}
void eventCallBack::readRequestHandler(connectionPtr pConn)
{
    cout << "触发读事件" << endl;
}
void eventCallBack::writeRequestHandler(connectionPtr pConn)
{
    cout << "触发写事件" << endl;
}
