//
// Created by Lenovo on 2022/5/31.
//

#include "netListener.h"
#include <connectPtr.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
bool netListener::penListeningSockets()
{
    int         isock;                  // socket
    sockaddr_in servAddr;               // 服务器的地址结构体
    servAddr.sin_family      = AF_INET; // 选择协议族为IPV4
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    for (int i = 0; i < mListenPortCount; ++i)
    {
        isock = socket(AF_INET, SOCK_STREAM, 0); //系统函数，成功返回非负描述符，出错返回-1
        if (isock == -1)
        {
            //其实这里直接退出，那如果以往有成功创建的socket呢？就没得到释放吧，当然走到这里表示程序不正常，应该整个退出，也没必要释放了
            return false;
        }
        int reuseaddr = 1; // 1:打开对应的设置项
        if (setsockopt(isock, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuseaddr, sizeof(reuseaddr))
            == -1)
        {
            close(isock); // 无需理会是否正常执行了
            return false;
        }
        //设置该socket为非阻塞
        if (!setNonBlocking(isock))
        {
            close(isock);
            return false;
        }
        servAddr.sin_port = htons(mPort);
        //绑定服务器地址结构体
        if (bind(isock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
        {
            close(isock);
            return false;
        }

        //开始监听
        if (listen(isock, LISTEN_BACKLOG) == -1)
        {
            close(isock);
            return false;
        }
        listeningPtr pListensocketitem = new listeningStruct();
        pListensocketitem->port        = mPort;         // 记录下所监听的端口号
        pListensocketitem->fd          = isock;         // 套接字木柄保存下来
        mListenSocketList.push_back(pListensocketitem); // 加入到队列中
    }
    if (mListenSocketList.empty()) // 不可能一个端口都不监听吧
        return false;
    return true;
}
netListener::netListener(int port, int listenPortCount) :
    mPort(port), mListenPortCount(listenPortCount)
{
    penListeningSockets();
}
bool netListener::setNonBlocking(int sockfd)
{
    int nb = 1;                            // 0：清除，1：设置
    if (ioctl(sockfd, FIONBIO, &nb) == -1) // FIONBIO：设置/清除非阻塞I/O标记：0：清除，1：设置
    {
        return false;
    }
    return true;
}