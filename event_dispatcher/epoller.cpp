#include "epoller.h"

Epoller::Epoller(int Maxevent)
	: epollFd(epoll_create(1))
	, events(Maxevent) {  // events������Maxevent��Ԫ�أ�ÿ��Ԫ�ؽ���ȱʡ��ֵ��ʼ����Ϊ���������г���Ԫ�ش���������Ԥ֪����Ԫ�ص�ֵ��Ҫ��̬��ȡ��ʱ�򣬿ɲ������ֳ�ʼ����ʽ
	assert(epollFd > 0 && events.size() > 0);
}

Epoller::~Epoller() {
	close(epollFd);
}

bool
Epoller::AddFd(int fd, uint32_t events) {
	if (fd < 0) return false;

	epoll_event event{NULL};
	event.data.fd = fd;
	event.events = events;

	return epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
}

bool
Epoller::DelFd(int fd, uint32_t events) {
	if (fd < 0) return false;

	epoll_event event{NULL};
	event.data.fd = fd;
	event.events = events;

	return epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, &event);
}

bool
Epoller::ModFd(int fd, uint32_t events) {
	if (fd < 0) return false;

	epoll_event event{ NULL };
	event.data.fd = fd;
	event.events = events;

	return epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event);
}