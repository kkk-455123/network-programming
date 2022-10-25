#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>

// ���ڲ����̳߳ع���
std::atomic<int> x;
std::mutex cout_lock;

class ThreadPool {
public:
	ThreadPool(size_t count = 8)
		: stop(false) 
	{
		std::cout << std::thread::hardware_concurrency() << " concurrent threads are supported.\n";
		// ��ʼ��count���߳�
		for (size_t i = 0; i < count; i++)
		{
			threads.emplace_back(
				std::thread(
					[this] {
						while (true) {
							std::unique_lock<std::mutex> locker(this->mtx);  // �����
							cv.wait(locker, [this] {return this->stop || !tasks.empty(); });  // ���������������ͷ�������������������������ִ��
							if (this->stop && tasks.empty()) return;  // ���̳߳ؼ���ֹͣ��������ȫ����ɣ����̷߳���(����)

							std::function<void()> task = std::move(this->tasks.front());  // ȡ��������ж�ͷ���񡣿����Զ������Ƶ�auto�滻std::function<void()> task
							this->tasks.pop();
							locker.unlock();  // �Զ��еĲ�����ɣ��ͷ���

							task();  // ִ������

							locker.lock();
						}
					}
				)
			);
		}
	}

	~ThreadPool() {
		{
			std::lock_guard<std::mutex> locker(this->mtx);  // ��������������stop��־λ�� ��Ϊ���̹߳����ĺ�����Ҳ��Ҫ����stop
			stop = true;
		}
		cv.notify_all();  // ���������̡߳������޸���stopΪtrue�������߳���wait�ĵڶ������������������㣬�������ڸ�ԭ������
		for (auto& thread : this->threads) {
			if (thread.joinable())
				thread.join();  // �ȴ������߳�ִ�����(����)
		}
	}

	// Ŀ��:ʵ���ܴ������������
	template<typename T>
	void AddTask(T&& task) {
		{
			std::lock_guard<std::mutex> locker(this->mtx);  // ���������������������tasks
			tasks.push(std::forward<T&&>(task));  // ת��Ϊ��ֵ
		}
		cv.notify_one();  // ��������񣬻����߳�ִ������
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

		std::lock_guard<std::mutex> locker(cout_lock);  // Ϊcout��������ֹ�������˳�����
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