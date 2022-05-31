//
// Created by Lenovo on 2022/5/30.
//

#ifndef EPOLLTCP_COMM_H
#define EPOLLTCP_COMM_H
#ifndef NGINX_LOGIC_COMM_H
#define NGINX_LOGIC_COMM_H

#include <cstdint>

#define PKG_MAX_LENGTH                                                                             \
    30000 //每个包的最大长度【包头+包体】不超过这个数字，为了留出一些空间，实际上编码是，包头+包体长度必须不大于
//这个值-1000【29000】

//通信 收包状态定义
#define PKG_HD_INIT 0    //初始状态，准备接收数据包头
#define PKG_HD_RECVING 1 //接收包头中，包头不完整，继续接收中
#define PKG_BD_INIT 2    //包头刚好收完，准备接收包体
#define PKG_BD_RECVING 3 //接收包体中，包体不完整，继续接收中，处理后直接回到_PKG_HD_INIT状态
//#define _PKG_RV_FINISHED     4  //完整包收完，这个状态似乎没什么 用处


#define SERVER_FILE_PATH "./" // 服务器的文件地址
#define LISTEN_BACKLOG 511

// 已完成连接队列，nginx给511，我们也先按照这个来：不懂这个数字的同学参考第五章第四节
#define MAX_EVENTS 512
#pragma pack(1) //对齐方式,1字节对齐【结构之间成员不做任何字节对齐：紧密的排列在一起】
typedef struct commPkgHeader
{
	unsigned short pkgLen;
	// 报文总长度【包头+包体】--2字节，2字节可以表示的最大数字为6万多，我们定义_PKG_MAX_LENGTH
	// 30000，所以用pkgLen足够保存下
	// 包头中记录着整个包【包头—+包体】的长度

	unsigned short msgCode; // 消息类型代码--2字节，用于区别每个不同的命令【不同的消息】
	unsigned short broadcast; // 群发
	unsigned char sessionId[16] = {0};
	int crc32;
	// CRC32效验--4字节，为了防止收发数据中出现收到内容和发送内容不一致的情况，引入这个字段做一个基本的校验用
} commPkgHeader, *lpcommPkgHeader;

#pragma pack() //取消指定对齐，恢复缺省对齐

#define PKG_HEADER_SIZE sizeof(commPkgHeader)
#endif // NGINX_LOGIC_COMM_H
#endif //EPOLLTCP_COMM_H
