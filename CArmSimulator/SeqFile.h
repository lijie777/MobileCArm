#pragma once

#include <string>

#include "common.h"
#include "SeqFileBase.h"

class SeqFile : public SeqFileBase
{
public:
    SeqFile(const std::string& name, SeqUnit_t* first, SeqUnit_t* last);
    ~SeqFile();

    enum HeadElement
    {
        FRAME_WIDTH = 0,
        FRAME_HEIGHT,
        FRAME_COUNT,
        SCAN_MODE,
        FOV_MODE
    };

private:
    // 获取每帧的字节数
    size_t frameSize_Byte() const override;
    // 此文件中包含多少帧
    size_t frameCount() const override;
    // 返回一段此文件配置的描述
    std::string description() const override;

    SeqFile* clone() const override { return new SeqFile(*this); }

    //explicit operator bool() const noexcept;
    size_t fileOffset() const override { return 2048 + m_frameIndex*frameSize_Byte(); }

private:
    const std::vector<SeqUnit_t>    m_head;     // seq数据头
};