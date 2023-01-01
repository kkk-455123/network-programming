/*
using native socket
*/

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

	// 1.创建客户端socket
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		cerr << "socket error" << endl;
		return -1;
	}

	// 2.向服务器发起连接请求
	struct hostent* h;
	if ((h = gethostbyname(argv[1])) == 0) {  // 识别ip地址或域名都可以
		cerr << "gethostbyname failed" << endl;
		close(sockfd);
		return -1;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	//servaddr.sin_addr.s_addr = inet_addr(argv[1]);  // 使用这种方式无法用域名，只能用ip地址
	memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		cerr << "connect error" << endl;
		close(sockfd);
		return -1;
	}

	// 3.与服务器通信，发送报文，收到回复后再发下一个报文
	char buffer[1024];
	while(true)
	{
		int iret;
		memset(buffer, 0, sizeof(buffer));
		
		string sendMessage;
		getline(cin, sendMessage);  // 直接用cin会跳过空格，造成how are you被分成三部分分别送入键盘缓存区
		strcpy(buffer, sendMessage.c_str());
		//sprintf(buffer, "how are you");

		// 调用一次send
		if ((iret = send(sockfd, buffer, strlen(buffer), 0)) <= 0) {
			cerr << "send error" << endl;
			break;
		}
		// 调用多次send
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

	// 4.关闭socket，释放资源
	close(sockfd);
}
