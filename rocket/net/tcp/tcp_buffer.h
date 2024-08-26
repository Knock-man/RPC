#pragma once
#include <vector>
#include <memory>
namespace rocket
{
    class TcpBuffer
    {
    public:
        typedef std::shared_ptr<TcpBuffer> s_ptr;

        TcpBuffer(int size);

        ~TcpBuffer();

        // 缓冲区写入size大小的数据
        void writeToBuffer(const char *buf, int size);

        // 读取缓冲区size大小的数据
        void readFromBuffer(std::vector<char> &re, int size);

        // 向后移动可读索引
        void moveReadIndex(int size);

        // 向右移动可写索引
        void moveWriteIndex(int size);

        // 返回可读字节数
        int readAble();

        // 返回可写字节数
        int writeAble();

        // 调整buffer大小
        void resizeBuffer(int new_size);

        // 获取可读索引
        int readIndex();

        // 获取可写索引
        int writeIndex();

    private:
        // 调整缓冲区向前滑动(删除已读数据，腾出空间)
        void adjustBuffer();

    private:
        int m_read_index{0};
        int m_write_index{0};
        int m_size{0};

    public:
        std::vector<char> m_buffer;
    };
}