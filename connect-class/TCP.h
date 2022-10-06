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

#include <iostream>
#include <string>

class TcpServer
{
public:
	TcpServer();
	
	bool InitServer(const int port);  // ��ʼ��������
	bool Accept();  // �ȴ��ͻ�������

	int Send(const void* const buf, const int buflen);  // ���ͱ���
	int Recv(void* const buf, const int buflen);  // ���ձ���

	void CloseListen();
	void CloseClient();

	~TcpServer();

private:
	int m_listenfd;  // ����socket
	int m_clientfd;  // ͨ��socket
	struct sockaddr_in m_serverAddr;  // ��������ַ
};

class TcpClient
{
public:
	TcpClient();

	bool ConnectToServer(const std::string ipAddr, const int port);

	int Send(const void* const buf, const int buflen);  // ���ͱ���
	int Recv(void* const buf, const int buflen);  // ���ձ���

	~TcpClient();

private:
	int m_sockfd;
	struct sockaddr_in m_servAddr;
};