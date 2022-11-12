#include <sys/epoll.h>
#include <chrono>
#include <functional>
#include <set>
#include <memory>  // 智能指针
#include <iostream>

using namespace std;

/*
定时节点的数据结构
func 回调方法 一个函数对象
expire 任务到期时间
id  任务节点的唯一标识
*/
struct TimerNodeBase {
    time_t expire;
    int64_t id;
};

struct TimerNode : public TimerNodeBase {
    using Callback = std::function<void(const TimerNode& node)>;
    Callback func;
    // time_t expire;
    // int64_t id;  // 唯一标识任务节点

    // // 根据expire的大小确定在set中的节点存储顺序
    // bool operator < (const TimerNode& node) {
    //     if(expire < node.expire) return true;
    //     else if(expire > node.expire) return false;
    //     else return id < node.id;  // 若expire相同，则通过id大小决定,id小的放在红黑树左边，越小越早执行
    // }
};

// 优化：节点比较时若传入整个节点结构比较 则效率较低，因此将节点的关键比较参数id和expire单独抽象出来，每个节点继承这个基类。在比较时只传入基类对象，提高效率
// 利用多态，timernodebase timernode类型都可以作为参数
bool operator < (const TimerNodeBase& lhs, const TimerNodeBase& rhs) {
    // 先比较到期时间
    if(lhs.expire < rhs.expire) return true;
    else if(lhs.expire > rhs.expire) return false;
    // 若到期时间相同，则比较唯一标识id
    else return lhs.id < rhs.id; 
}

// 定时器类
class Timer {
public:
    // 获取当前时间
    // 使用steady_clock, 因为更稳定、在精度可以满足需求的情况下效率更高
    // 与system_clock相比，system_clock可能会由于手动更改、网络对时更新等原因发生变化，用于计算时间差不稳定
    // 与高精度steady_clock相比，效率更高
    static time_t GetTick() {
        // // 当前时间(ms)
        // auto sc = chrono::time_point_cast<chrono::milliseconds> (chrono::steady_clock::now());
        auto sc = chrono::steady_clock::now();
        // // 当前时间距离纪元时间的时间间隔
        auto temp = chrono::duration_cast<chrono::milliseconds> (sc.time_since_epoch());
        return temp.count();
    }

    // 生成递增的id
    static int64_t GenarateId() {
        return ++gid;
    }

    /*
        功能：添加定时器任务节点
        参数1 msec：到期时间
        参数2 func：任务对象
        return：新添加的节点
    */
    TimerNodeBase AddTimer(time_t msec, TimerNode::Callback func) {
        // 创建新的任务节点
        TimerNode tnode;
        // 设置节点参数
        tnode.expire = this->GetTick() + msec;  // 当前时间加上设置时间
        tnode.func = func;
        tnode.id = this->GenarateId();

        // 将节点添加到某个数据结构中
        timeMap.insert(tnode);
        // 返回新添加的定时器节点
        return static_cast<TimerNodeBase> (tnode);
    }

    // 删除定时器
    bool DelTimer(TimerNodeBase &node) {
         auto iter = timeMap.find(node);
         // 找到了该节点
         if(iter != timeMap.end()) {
            timeMap.erase(iter);
            return true;
         }
         return false;
    }

    // 检查定时器
    // 检查是否有任务节点到达设定时间
    bool CheckTimer() {
        // 先找到某个数据结构中最小（expire）的节点
        auto iter = timeMap.begin();
        if(iter != timeMap.end() && iter->expire <= GetTick()) {  // expire小于当前时间，表示到达设定时间  
            // 执行任务
            iter->func(*iter);
            timeMap.erase(iter);
            return true;
        }
        return false;
    }


    time_t TimeToSleep() {
        auto iter = timeMap.begin();
        if(iter == timeMap.end()) return -1;  // timeMap中没有节点 返回-1 即epoll一直阻塞
        time_t diss = iter->expire - this->GetTick();  // 定时时间和当前时间的间隔，即需要等待的时间
        return diss > 0 ? diss : 0;
    }

private:
    // 节点id值
    static int64_t gid;
    // set集合存放时间节点。set底层用红黑树实现,有序
    set<TimerNode, less<>> timeMap;

};

int64_t Timer::gid = 0;

int main() {
    int epfd = epoll_create1(1);  // 创建epoll
    epoll_event ev[64] = {0};

    unique_ptr<Timer> timer = make_unique<Timer>();
    timer->AddTimer(1000, [](const TimerNode &node) {cout<<"node id:"<<node.id<<endl;});
    timer->AddTimer(1000, [](const TimerNode &node) {cout<<"node id:"<<node.id<<endl;});

    while (true) {
        /*
            epoll_wait最后一个参数的说明
            -1 一直阻塞
            0 没有事件就立刻返回，有事件拷贝到ev数组中
            t > 0 阻塞等待 t ms
            此处设置为最近触发的定时任务距离当前时刻的间隔
        */
        int n = epoll_wait(epfd, ev, 64, timer->TimeToSleep());
        for (int i = 0; i < n; i++) {
            /* code */
            cout << "event processing" << endl;
        }
        while(timer->CheckTimer());
    }
    
    return 0;
}
