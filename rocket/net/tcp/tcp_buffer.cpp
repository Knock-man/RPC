#include <memory>
#include <string.h>
#include "tcp_buffer.h"
#include "../rocket/common/log.h"
namespace rocket
{
    TcpBuffer::TcpBuffer(int size) : m_size(size)
    {
        m_buffer.resize(size);
    }

    TcpBuffer::~TcpBuffer()
    {
    }

    // 返回可读字节数
    int TcpBuffer::readAble()
    {
        return m_write_index - m_read_index;
    }

    // 返回可写字节数
    int TcpBuffer::writeAble()
    {
        return m_size - m_write_index;
    }

    int TcpBuffer::readIndex()
    {
        return m_read_index;
    }

    int TcpBuffer::writeIndex()
    {
        return m_write_index;
    }

    void TcpBuffer::writeToBuffer(const char *buf, int size)
    {
        if (size > writeAble())
        {
            // 调整buffer的大小，扩容
            int new_size = (int)1.5 * (m_write_index + size);
            resizeBuffer(new_size);
        }
        memcpy(&m_buffer[m_write_index], buf, size);
        m_write_index += size;
    }

    void TcpBuffer::readFromBuffer(std::vector<char> &re, int size)
    {
        if (readAble() == 0)
            return;

        int read_size = readAble() > size ? size : readAble();

        std::vector<char> tmp(read_size);
        memcpy(&tmp[0], &m_buffer[m_read_index], read_size);
        re.swap(tmp);

        m_read_index += read_size;

        adjustBuffer();
    }

    // 调整Buffer大小
    void TcpBuffer::resizeBuffer(int new_size)
    {
        std::vector<char> tmp(new_size);
        int count = std::min(new_size, readAble());
        memcpy(&tmp[0], &m_buffer[m_read_index], count);
        m_buffer.swap(tmp);
        m_read_index = 0;
        m_write_index = m_read_index + count;
    }

    // 已经读过的数据大小大于Buffer内存的1/3，需要滑动缓冲区
    void TcpBuffer::adjustBuffer()
    {
        // 腾出的空间小于1/3 不用调整
        if (m_read_index < int(m_buffer.size() / 3))
            return;

        std::vector<char> buffer(m_buffer.size());
        int count = readAble();

        // 可读的数据拷贝到新数组
        memcpy(&buffer[0], &m_buffer[m_read_index], count);
        m_buffer.swap(buffer); // 拷贝回去

        // 调整可读可写的下标
        m_read_index = 0;
        m_write_index = m_read_index + count;

        buffer.clear();
    }

    // 向后移动可读索引
    void TcpBuffer::moveReadIndex(int size)
    {
        int j = m_read_index + size;
        if (j >= m_buffer.size())
        {
            ERRORLOG("moveReadIndex error, invalid size %d, old_read_index %d, buffer size %d", size, m_read_index, m_buffer.size());
            return;
        }
        m_read_index = j;
        adjustBuffer();
    }

    // 向右移动可写索引
    void TcpBuffer::moveWriteIndex(int size)
    {
        int j = m_write_index + size;
        if (j >= m_buffer.size())
        {
            ERRORLOG("moveReadIndex error, invalid size %d, old_read_index %d, buffer size %d", size, m_read_index, m_buffer.size());
            return;
        }
        m_write_index = j;
        adjustBuffer();
    }
}