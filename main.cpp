#include <iostream>
#include "epollServer/epollServer.h"
int main()
{
    epollServer *server = new epollServer();
    server->epollInit();
    while (true) { server->loop(); }
}
