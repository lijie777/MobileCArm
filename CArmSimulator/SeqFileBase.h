#pragma once
#include <string>

constexpr static size_t SEQ_FILE_HEADER_SIZE{ 2048 };

class SeqFileBase
{
public:
    SeqFileBase(const std::string& name)
        : m_filename(name)
        , m_frameIndex(0)
    {

    }

    // 获取当前的帧索引
    size_t frameIndex() const
    {
        return m_frameIndex;
    }

    // 获取seq文件名，含路径
    std::string filename() const
    {
        return m_filename;
    }

    // 此文件是否从未被读取过
    bool isNewFile() const
    {
        return (0 == m_frameIndex);
    }

    // 获取文件偏移量
    virtual size_t fileOffset() const = 0;

    // 此文件中包含多少帧
    virtual size_t frameCount() const = 0;

    virtual SeqFileBase* clone() const = 0;

    // 返回一段此文件配置的描述
    virtual std::string description() const = 0;

    // 获取每帧的字节数
    virtual size_t frameSize_Byte() const = 0;

    bool valid() const
    {
        return (m_frameIndex < frameCount());
    }

    // 将帧索引移动到下一帧
    void toNextFrame() const
    {
        if (valid())
        {
            ++m_frameIndex;
        }
    }

    // 当前的帧是否为丢帧
    virtual int CurrentFrameLost() const { return 0;}

protected:
    const std::string       m_filename;             // seq文件名
    mutable size_t          m_frameIndex;           // 数据帧索引
};
Q_DECLARE_METATYPE(SeqFileBase*)