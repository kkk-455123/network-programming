#include "epoller.h"

Epoller::Epoller(int Maxevent)
	: m_epollFd(epoll_create(1))
	, m_events(Maxevent) {  // events������Maxevent��Ԫ�أ�ÿ��Ԫ�ؽ���ȱʡ��ֵ��ʼ����Ϊ���������г���Ԫ�ش���������Ԥ֪����Ԫ�ص�ֵ��Ҫ��̬��ȡ��ʱ�򣬿ɲ������ֳ�ʼ����ʽ
	assert(m_epollFd > 0 && m_events.size() > 0);
}

Epoller::~Epoller() {
	close(m_epollFd);
}

bool
Epoller::AddFd(int fd, uint32_t events) {
	if (fd < 0) return false;

	epoll_event event{NULL};
	event.data.fd = fd;
	event.events = events;

	return epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &event);
}

bool
Epoller::DelFd(int fd) {
	if (fd < 0) return false;

	epoll_event event{NULL};

	return epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &event);
}

bool
Epoller::ModFd(int fd, uint32_t events) {
	if (fd < 0) return false;

	epoll_event event{ NULL };
	event.data.fd = fd;
	event.events = events;

	return epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &event);
}

int 
Epoller::Wait(int timeoutMs) {
	return epoll_wait(m_epollFd, m_events.data(), static_cast<int>(m_events.size()), timeoutMs);
}	// data()����vector������Ԫ�ص�ַ

int
Epoller::GetEventFd(size_t i) const {
	assert(i >= 0 && i < m_events.size());
	return m_events[i].data.fd;
}

uint32_t Epoller::GetEvent(size_t i) const {
	assert(i >= 0 && i < m_events.size());
	return m_events[i].events;
}