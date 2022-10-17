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
			// error�¼�
			if ((event_event & EPOLLERR)
				|| (event_event & EPOLLHUP)
				|| (!(event_event & EPOLLIN))) {
				perror("epoll error");
				close(event_fd);
				continue;
			}

			// �¼�����listenfd
			else if (listenfd == event_fd) {
				int clientfd = server.Accept(listenfd);  // ��ȡ�����׽���
				make_nonblocking(clientfd);  // ���÷�����
				epoller.AddFd(clientfd, EPOLLIN | EPOLLET);  // �������׽��ּ�����
				continue;
			}

			// �¼����Կͻ���
			else {
				int socket_fd = event_fd;
				std::cout << "get event on socket fd == " << socket << std::endl;
				// ��������
				//while (1) {
				//	char buf[512];
				//	// �����ݳ���
				//	if ((n = read(socket_fd, buf, sizeof(buf))) < 0) {
				//		if (errno != EAGAIN) {
				//			perror("read error");
				//			close(socket_fd);
				//		}
				//		break;
				//	}
				//	// �ͻ��������ر�, �ر������׽��� 
				//	else if (n == 0) {
				//		close(socket_fd);
				//		break;
				//	}
				//	// ���ݴ����߼�and��������
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