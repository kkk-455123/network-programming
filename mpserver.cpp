#include "TCP.h"
using namespace std;

int main()
{
	signal(SIGCHLD, SIG_IGN);  // �����ӽ����˳��źţ���ʾ�����̲����ģ����佻��init���̹���
	TcpServer server;

	if (server.InitServer(21567) == false) { cout << "Init failed,exit..." << endl; return -1; }
	while (true) {
		if (server.Accept() == false) { cout << "Accept failed,exit..." << endl; continue; }
		if (fork() > 0) { // �����ӽ���
			server.CloseClient();  // �����̹ر�����ͨ�ŵ�socket
			continue;  // �����̼�������
		}
		
		// �ӽ��̼���ִ�г���
		{
			server.CloseListen();  // �ӽ��̲���Ҫ���ڼ�����socket
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
			exit(0);  // �˳���ǰ����
		}
	}
}