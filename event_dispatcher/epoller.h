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
	// 向epoll实例注册文件描述符fd需要被监控的事件
	bool AddFd(int fd, uint32_t events);
	// 在epoll实例中删除文件描述符fd对应的事件
	bool DelFd(int fd, uint32_t events);
	// 修改epoll实例中文件描述符fd对应的事件
	bool ModFd(int fd, uint32_t events);

	// 等待内核I/O事件分发
	int Wait(int timeoutMs = -1);

private:
	int epollFd;  // epoll实例，用来调用epoll_ctl和epoll_wait
	// 服务器关闭后需要调用close()方法释放epoll实例

	std::vector<struct epoll_event> events;  // 元素为一个结构体，表示注册的事件类型，其中最常用的是fd字段，表示事件对应的文件描述符
};