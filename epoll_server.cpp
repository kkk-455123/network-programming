/*
echo server
using epoll
using TCP.h and epoller.h
*/

#include <iostream>
#include "./connect-class/TCP.h"
#include "./event_dispatcher/epoller.h"

char CharChange(char c) {
	if (c >= 'a' && c < 'h') return 'a';
	else if (c > 'k' && c <= 'z') return 'z';
	else return 'h';
}

int main() {
	TcpServer server;
	int listenfd = server.InitServer(21567);

	Epoller epoller;
	epoller.AddFd(listenfd, EPOLLIN | EPOLLET);
	
	int n;
	while (1)
	{
		n = epoller.Wait();
		std::cout << "epoll_wait wakeup" << std::endl;
		for (size_t i = 0; i < n; i++)
		{
			std::cout << "start processing event:" << i << std::endl;
			int event_fd = epoller.GetEventFd(i);
			uint32_t event_event = epoller.GetEvent(i);
			// error事件
			if ((event_event & EPOLLERR)
				|| (event_event & EPOLLHUP)
				|| (!(event_event & EPOLLIN))) {
				perror("epoll error");
				close(event_fd);
				continue;
			}

			// 事件来自listenfd
			else if (listenfd == event_fd) {
				int clientfd = server.Accept(listenfd);  // 获取连接套接字
				make_nonblocking(clientfd);  // 设置非阻塞
				epoller.AddFd(clientfd, EPOLLIN | EPOLLET);  // 将连接套接字加入监测
				continue;
			}

			// 事件来自客户端
			else {
				int socket_fd = event_fd;
				std::cout << "get event on socket fd == " << socket << std::endl;
				// 接收数据
				while (1) {
					char buf[512];
					int n_read = read(socket_fd, buf, sizeof(buf));
					// 读数据出错
					if (n_read < 0) {
						if (errno != EAGAIN) {
							perror("read error");
							close(socket_fd);
						}
						break;
					}
					// 客户端正常关闭, 关闭连接套接字 
					else if (n_read == 0) {
						std::cout << "close normally." << std::endl;
						close(socket_fd);
						break;
					}
					// 数据处理逻辑and发送数据
					else {
						if (write(socket_fd, buf, n_read) < 0) perror("write error");
						std::cout << "send finish." << std::endl;
					}
				}
			}
			
		}
	}

	return 0;
}
