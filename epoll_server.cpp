#include <iostream>
#include "./network-programming/connect-class/TCP.h"
#include "./network-programming/event_dispatcher/epoller.h"

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
				//while (1) {
				//	char buf[512];
				//	// 读数据出错
				//	if ((n = read(socket_fd, buf, sizeof(buf))) < 0) {
				//		if (errno != EAGAIN) {
				//			perror("read error");
				//			close(socket_fd);
				//		}
				//		break;
				//	}
				//	// 客户端正常关闭, 关闭连接套接字 
				//	else if (n == 0) {
				//		close(socket_fd);
				//		break;
				//	}
				//	// 数据处理逻辑and发送数据
				//	else {
				//		for (int i = 0; i < n; i++) {
				//			buf[i] = CharChange(buf[i]);
				//		}
				//		if (write(socket_fd, buf, n) < 0) perror("write error");
				//	}
				//}
			}
			
		}
	}

	return 0;
}