#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Using:./server port !" << std::endl;
		return -1;
	}

	// 1.创建服务端socket
	int listenfd;
	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  // 赋值符号优先级最低，注意给赋值表达式加括号
		std::cerr << "socket error" << std::endl;
		return -1;
	}

	// 2.绑定服务端ip地址和端口
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;  // 协议族
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // 指定主机上的任意ip地址
	servaddr.sin_port = htons(atoi(argv[1]));  // 通信端口
	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		perror("bind");
		//std::cerr << "bind error" << std::endl;
		//std::cerr << "error:" << strerror(errno) << std::endl;  // 获取错误信息
		return -1;
	}

	// 3.设置socket为监听模式
	// listenfd socket只负责监听，不负责和客户端通信
	if (listen(listenfd, 5) != 0) { 
		perror("listen");
		close(listenfd);
		return -1;
	}

	// 4.接受客户端连接
	int clientfd;  // 客户端socket
	int socklen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientaddr;
	// 从已准备好的连接队列中获取一个请求，若队列为空会阻塞等待
	clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, (socklen_t*)&socklen);  // 如果不需要知道客户端ip，后两个参数可以为null
	std::cout << "Client" << inet_ntoa(clientaddr.sin_addr) << "has connected" << std::endl;
	
	// 5.接收客户端发送的报文
	char buffer[1024];
	while (true)
	{
		int iret;
		memset(buffer, 0, sizeof(buffer));
		if ((iret = recv(clientfd, buffer, sizeof(buffer), 0)) <= 0) {
			std::cout << "iret=" << iret << std::endl;
			break;
		}
		if (std::string((const char*)buffer) == "bye") {
			std::cout << "exit" << std::endl;
			break;
		}
		std::cout << "Receive client Message:" << buffer << std::endl;
			
		std::string sendMessage;
		getline(std::cin, sendMessage);
		strcpy(buffer, sendMessage.c_str());

		if ((iret = send(clientfd, buffer, strlen(buffer), 0)) <= 0) {
			perror("send");
			break;
		}
		std::cout << "Send" << iret << ":" << buffer << std::endl;
	}

	// 6.关闭socket 释放资源
	close(listenfd);
	close(clientfd);
}