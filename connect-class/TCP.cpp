#include "TCP.h"

TcpServer::TcpServer() : m_listenfd(0), m_clientfd(0) {}

TcpServer::~TcpServer() {
	if (m_listenfd != 0) close(m_listenfd);
	if (m_clientfd != 0) close(m_clientfd);
}

//bool 
//TcpServer::InitServer(const int port) {
//	if((m_listenfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) return false;
//	make_nonblocking(m_listenfd);  // �Ѽ����׽�������Ϊ������
//
//	m_serverAddr.sin_family = AF_INET;
//	m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	m_serverAddr.sin_port = htons(port);
//	if (bind(m_listenfd, (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr)) != 0) {
//		close(m_listenfd);
//		m_listenfd = 0;
//		return false;
//	}
//
//	if (listen(m_listenfd, 5) != 0) {
//		close(m_listenfd);
//		m_listenfd = 0;
//		return false;
//	}
//
//	return true;
//}

int 
TcpServer::InitServer(const int port) {
	m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	make_nonblocking(m_listenfd);  // �Ѽ����׽�������Ϊ������

	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_serverAddr.sin_port = htons(port);

	// ����˿����ã��Ա����˳����ڼ��̵�ʱ��������ʱ���Ը���ͬһ���˿�
	int on = 1;
	setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  // SO_REUSEADDRѡ��

	int rt1 = bind(m_listenfd, (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr));
	if (rt1 < 0) { perror("bind failed"); }

	int rt2 = listen(m_listenfd, 5);
	if (rt2 < 0) { perror("listen failed"); }

	signal(SIGPIPE, SIG_IGN);  // ��һ���Զ��Ѿ��رյ�socket��������write, �ڶ��ν�������SIGPIPE�ź�, ���ź�Ĭ�Ͻ������̡�Ϊ�˱�������˳����������ø��źŴ�����������

	return m_listenfd;
}

//bool
//TcpServer::Accept() {
//	m_clientfd = accept(m_listenfd, NULL, NULL);
//	if (m_clientfd <= 0) { perror("accept"); return false;  }
//	return true;
//}

int
TcpServer::Accept(int listenfd) {
	int fd = accept(listenfd, NULL, NULL);
	if (fd < 0) perror("accept failed");
	return fd;
}

int
TcpServer::Send(const void* const buf, const int buflen) {
	return send(m_clientfd, buf, buflen, 0);
}

int
TcpServer::Recv(void* const buf, const int buflen) {
	return recv(m_clientfd, buf, buflen, 0);
}

void
TcpServer::CloseListen() {
	if (m_listenfd != 0) {
		close(m_listenfd); m_listenfd = 0;
	}
}

void 
TcpServer::CloseClient() {
	if (m_clientfd != 0) {
		close(m_clientfd); m_clientfd = 0;
	}
}

TcpClient::TcpClient() : m_sockfd(0) {}
TcpClient::~TcpClient() {
	if (m_sockfd != 0) close(m_sockfd);
}

bool 
TcpClient::ConnectToServer(const std::string ipAddr, const int port) {
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct hostent* h = gethostbyname(ipAddr.c_str());
	if (h == 0) { close(m_sockfd); m_sockfd = 0; return false; }

	m_servAddr.sin_family = AF_INET;
	m_servAddr.sin_port = htons(port);
	memcpy(&m_servAddr.sin_addr, h->h_addr, h->h_length);

	if (connect(m_sockfd, (struct sockaddr*)&m_servAddr, sizeof(m_servAddr)) != 0) {
		close(m_sockfd); m_sockfd = 0; return false;
	}

	return true;
}

int
TcpClient::Send(const void* const buf, const int buflen) {
	return send(m_sockfd, buf, buflen, 0);
}

int
TcpClient::Recv(void* const buf, const int buflen) {
	return recv(m_sockfd, buf, buflen, 0);
}

void make_nonblocking(int fd) {
	fcntl(fd, F_SETFL, O_NONBLOCK);
}