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

	// 1.���������socket
	int listenfd;
	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  // ��ֵ�������ȼ���ͣ�ע�����ֵ���ʽ������
		std::cerr << "socket error" << std::endl;
		return -1;
	}

	// 2.�󶨷����ip��ַ�Ͷ˿�
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;  // Э����
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // ָ�������ϵ�����ip��ַ
	servaddr.sin_port = htons(atoi(argv[1]));  // ͨ�Ŷ˿�
	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		perror("bind");
		//std::cerr << "bind error" << std::endl;
		//std::cerr << "error:" << strerror(errno) << std::endl;  // ��ȡ������Ϣ
		return -1;
	}

	// 3.����socketΪ����ģʽ
	// listenfd socketֻ���������������Ϳͻ���ͨ��
	if (listen(listenfd, 5) != 0) { 
		perror("listen");
		close(listenfd);
		return -1;
	}

	// 4.���ܿͻ�������
	int clientfd;  // �ͻ���socket
	int socklen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientaddr;
	// ����׼���õ����Ӷ����л�ȡһ������������Ϊ�ջ������ȴ�
	clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, (socklen_t*)&socklen);  // �������Ҫ֪���ͻ���ip����������������Ϊnull
	std::cout << "Client" << inet_ntoa(clientaddr.sin_addr) << "has connected" << std::endl;
	
	// 5.���տͻ��˷��͵ı���
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

	// 6.�ر�socket �ͷ���Դ
	close(listenfd);
	close(clientfd);
}