#include "epoller.h"

Epoller::Epoller(int Maxevent)
	: epollFd(epoll_create(1))
	, events(Maxevent) {  // events将包含Maxevent个元素，每个元素进行缺省的值初始化。为当程序运行初期元素大致数量可预知，而元素的值需要动态获取的时候，可采用这种初始化方式
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