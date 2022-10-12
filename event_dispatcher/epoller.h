#pragma once
#include <sys/epoll.h>
#include <vector>
#include <cassert>
#include <unistd.h>

class Epoller {
public:
	explicit Epoller(int maxEvent = 1024);
	~Epoller();

	/*epoll_ctl*/
	// ��epollʵ��ע���ļ�������fd��Ҫ����ص��¼�
	bool AddFd(int fd, uint32_t events);
	// ��epollʵ����ɾ���ļ�������fd��Ӧ���¼�
	bool DelFd(int fd, uint32_t events);
	// �޸�epollʵ�����ļ�������fd��Ӧ���¼�
	bool ModFd(int fd, uint32_t events);

	// �ȴ��ں�I/O�¼��ַ�
	int Wait(int timeoutMs = -1);

private:
	int epollFd;  // epollʵ������������epoll_ctl��epoll_wait
	// �������رպ���Ҫ����close()�����ͷ�epollʵ��

	std::vector<struct epoll_event> events;  // Ԫ��Ϊһ���ṹ�壬��ʾע����¼����ͣ�������õ���fd�ֶΣ���ʾ�¼���Ӧ���ļ�������
};