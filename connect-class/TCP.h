#pragma once
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <csignal>
#include <fcntl.h>

#include <iostream>
#include <string>

class TcpServer
{
public:
	TcpServer();
	
	//bool InitServer(const int port);  // 初始化服务器
	int InitServer(const int port);  // 初始化服务器，并返回监听套接字
	//bool Accept();  // 等待客户端连接
	int Accept(int listenfd);  // 等待客户端连接，返回连接套接字

	int Send(const void* const buf, const int buflen);  // 发送报文
	int Recv(void* const buf, const int buflen);  // 接收报文

	void CloseListen();
	void CloseClient();

	~TcpServer();

private:
	int m_listenfd;  // 监听socket
	int m_clientfd;  // 通信socket
	struct sockaddr_in m_serverAddr;  // 服务器地址
};

class TcpClient
{
public:
	TcpClient();

	bool ConnectToServer(const std::string ipAddr, const int port);

	int Send(const void* const buf, const int buflen);  // 发送报文
	int Recv(void* const buf, const int buflen);  // 接收报文

	~TcpClient();

private:
	int m_sockfd;
	struct sockaddr_in m_servAddr;
};

// 设置非阻塞
void make_nonblocking(int);