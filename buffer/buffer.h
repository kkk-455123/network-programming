#ifndef BUFFER_H
#define BUFFER_H

#include <atomic>  // 原子变量
#include <vector>  // buffer变量
#include <iostream>
#include <cassert>

#include <sys/uio.h>  // readv
#include <unistd.h>  // write  

#include <cstring>  // bzero

class Buffer {
public:
    // 构造析构函数
    Buffer(int);
    ~Buffer() = default;

    // 查询属性信息的接口

    // 查询缓冲区可读写大小
    size_t ReadableBytes() const;
    size_t WritableBytes() const;
    size_t PrependableBytes() const;

    // 查询读写指针的具体位置(非偏移量，而是实际地址)
    char* BeginWrite();
    char* BeginRead();

    // 查询buffer起始地址
    char* BeginPtr();


    // 对buffer进行操作修改的接口

    // 读写操作,buffer和套接字数据的传输,对原生读写函数的封装
    ssize_t ReadFd(int fd, int* Errno);
    ssize_t WriteFd(int fd, int* Errno);

    // 扩充空间
    void MakeSpace(size_t len);

    // 向buffer追加数据
    void Append(const char*, size_t len);

    // 清空缓冲区
    void RetriveAll();
    std::string RetriveAllToStr();

private:
    std::vector<char> buffer_;
    std::atomic_size_t readPos_;
    std::atomic_size_t writePos_;
};

#endif //BUFFER_H
