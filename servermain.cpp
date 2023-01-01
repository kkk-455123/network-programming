/*
echo server
basic socket网络模型，一对一通信
使用自己封装的socket连接类
*/

#include "TCP.h"
using namespace std;

int main()
{
	TcpServer server;
	
	if (server.InitServer(21567) == false) { cout << "Init failed,exit..." << endl; return -1; }
	if (server.Accept() == false) { cout << "Accept failed,exit..." << endl; return -1; }
	cout << "Connected client." << endl;

	const int BufSize = 1024;
	char buffer[BufSize];

	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		if (server.Recv(buffer, sizeof(buffer)) <= 0 || string(buffer) == "bye") break;
		cout << "Client Message :" << buffer << endl;

		string sendMsg;
		getline(cin, sendMsg);
		strcpy(buffer, sendMsg.c_str());
		if (server.Send(buffer, strlen(buffer)) <= 0) break;
	}

	cout << "Disconnected." << endl;
}
