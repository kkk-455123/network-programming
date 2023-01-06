#include "buffer.h"

// 构造函数
Buffer::Buffer(int initialSize)
    : buffer_(initialSize)
    , readPos_(0), writePos_(0) {}

size_t Buffer::ReadableBytes() const {
    return writePos_ - readPos_;  // 因为需要的是字节数，所以用偏移量计算即可
}

size_t Buffer::WritableBytes() const {
    return buffer_.size() - writePos_;
}

size_t Buffer::PrependableBytes() const {
    return readPos_;
}

char* Buffer::BeginWrite() {
    return this->BeginPtr() + writePos_;
}

char* Buffer::BeginRead() {
    return this->BeginPtr() + readPos_;
}


char* Buffer::BeginPtr() {
    // return buffer_.data();
    return &*buffer_.begin();
}


void Buffer::MakeSpace(size_t len) {
    buffer_.resize(buffer_.size() + len + 1);
    std::cout << "扩充空间" << std::endl;
}


void Buffer::Append(const char* str, size_t len) {
    // 预留空间可以容纳数据
    if(len < this->PrependableBytes()) {
        // 将原有可读数据拷贝至buffer头部
        size_t readable = writePos_ - readPos_;  // 可读数据长度
        std::copy(this->BeginRead(), this->BeginWrite(), this->BeginPtr());
        readPos_ = 0;
        // 添加新数据
        std::copy(str, str + len, this->BeginPtr() + readable);
        writePos_ = readable + len;

        std::cout << "追加至预留空间" << std::endl;
    }
    // 预留空间不足以容纳数据
    else {
        this->MakeSpace(len);
        std::copy(str, str + len, this->BeginWrite());
        writePos_ += len;

        std::cout << "扩产空间并追加数据" << std::endl;
    }
}

ssize_t Buffer::ReadFd(int fd, int *Errno) {
    assert(fd > 0);

    char buff[65536];

    struct iovec iov[2];
    iov[0].iov_base =  const_cast<char*>(this->BeginWrite());
    iov[0].iov_len = this->WritableBytes();
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);
    
    ssize_t len = readv(fd, iov, 2);
    // 读数据出错
    if(len < 0) {
        *Errno = errno;
        return len;
    }
    // 读取数据成功
    // 缓冲区可写空间可以容纳,数据全部读入buffer
    else if(static_cast<size_t>(len) < this->WritableBytes()) {
        writePos_ += len;
        std::cout << "空间充足" << std::endl;
    }
    // 缓冲区可写空间不足,多余的数据暂存在buff中
    else {
        writePos_ = buffer_.size();  // buffer可写空间写满
        Append(buff, len - this->WritableBytes());  // 将buff的数据写入buffer
    }
    return len;
}

// 将buffer的数据写入fd
// 即读取buffer数据到fd
ssize_t Buffer::WriteFd(int fd, int* Errno) {
    assert(fd > 0);
    ssize_t len = write(fd, this->BeginRead(), this->ReadableBytes());
    if(len < 0) *Errno = errno;
    else readPos_ += len;
    return len;
}

void Buffer::RetriveAll() {
    bzero(&buffer_.front(), buffer_.size());
    readPos_ = 0;
    writePos_ = 0;
}

std::string Buffer::RetriveAllToStr() {
    std::string temp(BeginRead(), BeginWrite());
    this->RetriveAll();
    return temp;
}
