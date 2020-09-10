#include <fstream>
#include <sstream>

#include "SeqFile.h"
#include "SeqFileParser.h"

SeqFile::SeqFile(const std::string& name, SeqUnit_t* first, SeqUnit_t* last)
    : SeqFileBase(name)
    , m_head(first, last)
{

}

SeqFile::~SeqFile()
{

}



// SeqFile::operator bool() const noexcept
// {
//     return (m_frameIndex < m_head[SeqFileParser::FRAME_COUNT]);
// }

// 获取每帧的字节数
size_t SeqFile::frameSize_Byte() const
{
    return m_head[SeqFile::FRAME_WIDTH] * m_head[SeqFile::FRAME_HEIGHT] * sizeof(SeqUnit_t);
}

// 此文件中包含多少帧
size_t SeqFile::frameCount() const
{
    return m_head[SeqFile::FRAME_COUNT];
}

// 返回一段此文件配置的描述
std::string SeqFile::description() const
{
    std::ostringstream o;
    o << "[" << fs::path(m_filename).filename() << "] "
        << "帧宽：" << m_head[SeqFile::FRAME_WIDTH]
        << ", 帧高：" << m_head[SeqFile::FRAME_HEIGHT]
        << ", 帧数：" << m_head[SeqFile::FRAME_COUNT]
        << ", 扫描模式：" << m_head[SeqFile::SCAN_MODE]
        << ", FOV模式：" << m_head[SeqFile::FOV_MODE];
    return o.str();
}
