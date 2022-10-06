#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cerr << "Using ./client ip port" << endl;
		return -1;
	}

	// 1.�����ͻ���socket
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		cerr << "socket error" << endl;
		return -1;
	}

	// 2.�������������������
	struct hostent* h;
	if ((h = gethostbyname(argv[1])) == 0) {  // ʶ��ip��ַ������������
		cerr << "gethostbyname failed" << endl;
		close(sockfd);
		return -1;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	//servaddr.sin_addr.s_addr = inet_addr(argv[1]);  // ʹ�����ַ�ʽ�޷���������ֻ����ip��ַ
	memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		cerr << "connect error" << endl;
		close(sockfd);
		return -1;
	}

	// 3.�������ͨ�ţ����ͱ��ģ��յ��ظ����ٷ���һ������
	char buffer[1024];
	while(true)
	{
		int iret;
		memset(buffer, 0, sizeof(buffer));
		
		string sendMessage;
		getline(cin, sendMessage);  // ֱ����cin�������ո����how are you���ֳ������ֱַ�������̻�����
		strcpy(buffer, sendMessage.c_str());
		//sprintf(buffer, "how are you");

		// ����һ��send
		if ((iret = send(sockfd, buffer, strlen(buffer), 0)) <= 0) {
			cerr << "send error" << endl;
			break;
		}
		// ���ö��send
		//int count = 0;
		//while (count < sendMessage.size())
		//{
		//	if ((iret = send(sockfd, buffer, strlen(buffer), 0)) <= 0) {
		//		cerr << "send error" << endl;
		//		break;
		//	}
		//	count += iret;
		//}
		cout << "Send message:" << buffer << endl;

		memset(buffer, 0, sizeof(buffer));
		if ((iret = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0) {
			cout << "iret=" << iret << endl;
			break;
		}
		if (std::string((const char*)buffer) == "bye") {
			std::cout << "exit" << std::endl;
			break;
		}
		cout << "Receive" << iret << ":" << buffer << endl;
	}

	// 4.�ر�socket���ͷ���Դ
	close(sockfd);
}