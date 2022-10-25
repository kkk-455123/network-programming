#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>

// 用于测试线程池工作
std::atomic<int> x;
std::mutex cout_lock;

class ThreadPool {
public:
	ThreadPool(size_t count = 8)
		: stop(false) 
	{
		std::cout << std::thread::hardware_concurrency() << " concurrent threads are supported.\n";
		// 初始化count个线程
		for (size_t i = 0; i < count; i++)
		{
			threads.emplace_back(
				std::thread(
					[this] {
						while (true) {
							std::unique_lock<std::mutex> locker(this->mtx);  // 获得锁
							cv.wait(locker, [this] {return this->stop || !tasks.empty(); });  // 若条件不满足则释放锁并阻塞，否则获得锁并继续执行
							if (this->stop && tasks.empty()) return;  // 若线程池即将停止，且任务全部完成，则线程返回(销毁)

							std::function<void()> task = std::move(this->tasks.front());  // 取出任务队列队头任务。可用自动类型推导auto替换std::function<void()> task
							this->tasks.pop();
							locker.unlock();  // 对队列的操作完成，释放锁

							task();  // 执行任务

							locker.lock();
						}
					}
				)
			);
		}
	}

	~ThreadPool() {
		{
			std::lock_guard<std::mutex> locker(this->mtx);  // 获得锁，互斥访问stop标志位。 因为在线程关联的函数中也需要访问stop
			stop = true;
		}
		cv.notify_all();  // 唤醒所有线程。由于修改了stop为true，所以线程中wait的第二个参数条件都能满足，不会由于该原因阻塞
		for (auto& thread : this->threads) {
			if (thread.joinable())
				thread.join();  // 等待所有线程执行完毕(销毁)
		}
	}

	// 目标:实现能传入参数的任务
	template<typename T>
	void AddTask(T&& task) {
		{
			std::lock_guard<std::mutex> locker(this->mtx);  // 获得锁，互斥访问任务队列tasks
			tasks.push(std::forward<T&&>(task));  // 转发为右值
		}
		cv.notify_one();  // 添加新任务，唤醒线程执行任务
	}

private:
	std::mutex mtx;
	std::condition_variable cv;
	bool stop;

	std::vector<std::thread> threads;
	std::queue< std::function<void()> > tasks;
};

void func1() {
	std::cout << "task1 running\n";
	for (size_t i = 1; i <= 10; i++)
	{
		x++;

		std::lock_guard<std::mutex> locker(cout_lock);  // 为cout加锁，防止输出内容顺序混乱
		std::cout << "task1 x:" << x << std::endl;
	}
	
}

void func2() {
	std::cout << "task2 running\n";
	for (size_t i = 1; i <= 10; i++)
	{
		x++;

		std::lock_guard<std::mutex> locker(cout_lock);
		std::cout << "task2 x:" << x << std::endl;
	}
}



int main() {
	ThreadPool pool(2);
	pool.AddTask(func1);
	pool.AddTask(func2);

	system("pause");
	return 0;
}