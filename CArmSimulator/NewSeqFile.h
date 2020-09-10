#pragma once

#include <map>

#include "SeqFileBase.h"

static constexpr size_t FRAME_HEADER_SIZE{ 256 };

template <typename HeadType, typename FRAME_HEAD_T>
class NewSeqFile : public SeqFileBase
{
public:
    NewSeqFile(const std::string& name, HeadType head) : SeqFileBase(name), m_head{ head } {}

private:
    // 此文件中包含多少帧
    size_t frameCount() const override
    {
        return m_head.nFrameNum;
    }

    NewSeqFile* clone() const override
    {
        return new NewSeqFile(*this);
    };

    // 返回一段此文件配置的描述
    std::string description() const override
    {
        static const std::map<UEGSeqType, char*> seqTypeName
        {
            { UEGSeqType::CEPH, "CEPH"},
            { UEGSeqType::CR, "CR"},
            { UEGSeqType::CT, "CT"},
            { UEGSeqType::DR, "DR"},
            { UEGSeqType::PAN, "PAN"},
            { UEGSeqType::NoFrameHeader, "NoFrameHeader" }
        };

        std::ostringstream o;
        o << "[" << fs::path(m_filename).filename() << "] "
            << "文件类型：" << seqTypeName.at(m_head.eType)
            << ", 帧宽：" << m_head.nWidth
            << ", 帧高：" << m_head.nHeight
            << ", 帧数：" << m_head.nFrameNum;
        return o.str();
    }

    // 获取每帧的字节数
    size_t frameSize_Byte() const override
    {
        return m_head.nWidth*m_head.nHeight * sizeof(uint16_t);
    }

    size_t fileOffset() const override
    {
        return (2048 + m_frameIndex*(frameSize_Byte()+ FRAME_HEADER_SIZE) + FRAME_HEADER_SIZE);
    }

    // 当前帧（帧头）在文件中的偏移
    size_t FrameOffset() const
    {
        return SEQ_FILE_HEADER_SIZE + m_frameIndex*(frameSize_Byte() + FRAME_HEADER_SIZE);
    }

    // 当前的帧是否为丢帧
    int CurrentFrameLost() const override
    {
        int result{0};
        if (std::ifstream ifs{ filename(), std::ios::binary })
        {
            ifs.seekg(FrameOffset());
            FRAME_HEAD_T frame;
            ifs.read((char*)&frame, sizeof(FRAME_HEAD_T));
            result = (frame.bLostFrame ? 1 : 0);
        }
        return result;
    }

private:
    const HeadType       m_head;
};