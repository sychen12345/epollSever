//
// Created by Lenovo on 2022/5/30.
//

#ifndef EPOLLTCP_COMM_H
#define EPOLLTCP_COMM_H


#include <cstdint>

#define PKG_MAX_LENGTH 30000                                                                            \
#define PKG_HD_INIT 0    //接收数据包头
#define PKG_HD_RECVING 1 //包头不完整，继续接收中
#define PKG_BD_INIT 2    //包头刚好收完
#define PKG_BD_RECVING 3 //接收包体中，包体不完整



#define SERVER_FILE_PATH "./" // 服务器的文件地址
#define LISTEN_BACKLOG 511

#define MAX_EVENTS 512
#pragma pack(1) // 对齐方式
typedef struct commPkgHeader
{
	unsigned short pkgLen;
	unsigned short msgCode; // 消息类型代码
	unsigned short broadcast; // 群发
	unsigned char sessionId[16] = {0};
	int crc32;
	
} commPkgHeader, *lpcommPkgHeader;

#pragma pack() //取消指定对齐，恢复缺省对齐

#define PKG_HEADER_SIZE sizeof(commPkgHeader)

#endif 
