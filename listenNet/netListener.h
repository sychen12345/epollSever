//
// Created by Lenovo on 2022/5/31.
//

#ifndef EPOLLTCP_NETLISTENER_H
#define EPOLLTCP_NETLISTENER_H
#include <vector>
typedef struct listeningStruct listeningStruct, *listeningPtr;

class netListener
{
public:
    netListener(int port = 5005, int listenPortCount = 1);
    /**
     * 开始监听
     * @return
     */
    bool penListeningSockets();
    /**
     * 设置非阻塞
     * @param sockfd
     * @return
     */
    // bool
    bool                       setNonBlocking(int sockfd);
    std::vector<listeningPtr> &getListenSocketList() { return mListenSocketList; }

private:
    // bool                         setNonBlocking(int sockfd);
    std::vector<listeningPtr> mListenSocketList; //监听套接字队列
    int                       mListenPortCount;
    int                       mPort;             // 端口号

};


#endif // EPOLLTCP_NETLISTENER_H
