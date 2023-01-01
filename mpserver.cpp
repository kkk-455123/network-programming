/*
echo server
using the multi-process network model
using TCP.h
*/

#include "TCP.h"
using namespace std;

int main()
{
	signal(SIGCHLD, SIG_IGN);  // 忽略子进程退出信号，表示父进程不关心，将其交由init进程管理
	TcpServer server;

	if (server.InitServer(21567) == false) { cout << "Init failed,exit..." << endl; return -1; }
	while (true) {
		if (server.Accept() == false) { cout << "Accept failed,exit..." << endl; continue; }
		if (fork() > 0) { // 创建子进程
			server.CloseClient();  // 父进程关闭用于通信的socket
			continue;  // 父进程继续监听
		}
		
		// 子进程继续执行程序
		{
			server.CloseListen();  // 子进程不需要用于监听的socket
			cout << "Connected client" << getpid() << endl;

			const int BufSize = 1024;
			char buffer[BufSize];

			while (true)
			{
				memset(buffer, 0, sizeof(buffer));
				if (server.Recv(buffer, sizeof(buffer)) <= 0 || string(buffer) == "bye") break;
				cout << "Client Message :" << buffer << endl;

				strcpy(buffer, "ok");
				if (server.Send(buffer, strlen(buffer)) <= 0) break;
			}

			cout << "Disconnected client" << getpid() << endl;
			exit(0);  // 退出当前进程
		}
	}
}
