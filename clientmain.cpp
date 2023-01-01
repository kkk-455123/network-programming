/*
using TCP.h
*/

#include "TCP.h"
using namespace std;

int main()
{
	TcpClient client;
	if (!client.ConnectToServer("127.0.0.1", 21567)) { cout << "Connect failed,exit..." << endl; return -1; }
	cout << "Connected server." << endl;

	const int BufSize = 1024;
	char buffer[BufSize];

	for (int i = 0; i < 10; i++) {
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "这是第%d条消息\n", i);
		if (client.Send(buffer, strlen(buffer)) <= 0) break;
	}

	//while (true) {
	//	memset(buffer, 0, sizeof(buffer));
	//	
	//	string sendMsg;
	//	getline(cin, sendMsg);
	//	strcpy(buffer, sendMsg.c_str());
	//	
	//	if (client.Recv(buffer, sizeof(buffer)) <= 0 || string(buffer) == "bye") break;
	//	cout << "Server Message:" << buffer << endl;
	//}

	cout << "Disconnected." << endl;
}
