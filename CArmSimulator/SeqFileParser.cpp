#include <fstream>

#include "SeqFileParser.h"
#include "SeqFile.h"
#include "NewSeqFile.h"
#include "common.h"
#include "CommonUtility.h"
#include "SeqFileDef.h"

// seq文件集合
static SeqFileVec_t seqFiles;

// 被解析文件是否准备好
bool SeqFileParser::dataDirCheck()
{
    std::string seqDir = UCommonUtility::GetExeDir() + SEQ_FILE_DIR;
    return UEG_DIR::exists(seqDir) && !UEG_DIR::empty(seqDir);
}

// 获取所有的seq文件
const SeqFileVec_t& SeqFileParser::getSeqVec()
{
    return seqFiles;
}

// 遍历seq存放目录下的文件并解析
bool SeqFileParser::parseSeq()
{
    if (!dataDirCheck())
    {
        return false;
    }

    std::string seqDir = UCommonUtility::GetExeDir() + SEQ_FILE_DIR;

    std::vector<std::string> fileVec;
    for (const auto& i : fs::directory_iterator(seqDir))
    {
        fileVec.push_back(i.path().string());
    }

    for (const auto& file : fileVec)
    {
        parseNewTypeSeq(file);
    }

    return true;
}

bool SeqFileParser::parseNewTypeSeq(const std::string& file)
{
    if (std::ifstream ifs{ file, std::ios::binary })
    {
        char data[SEQ_FILE_HEADER_SIZE];
        if (ifs.read(data, sizeof(data)))
        {
            auto ptr = (SeqFileHeaderCT*)data;
            ifs.seekg(0, std::ifstream::end);
            const size_t size0 = ifs.tellg();
            const size_t size1 = ptr->nFrameNum * ptr->nWidth * ptr->nHeight * sizeof(uint16_t) + SEQ_FILE_HEADER_SIZE + ptr->nFrameNum*FRAME_HEADER_SIZE;
            if (size0 >= size1)
            {
                if (UEGSeqType::PAN == ptr->eType)
                {
                    seqFiles.push_back(new NewSeqFile<SeqFileHeaderPAN, FrameFileHeaderPAN>(file, *(SeqFileHeaderPAN*)ptr));
                }
                else if (UEGSeqType::CEPH == ptr->eType)
                {
                    seqFiles.push_back(new NewSeqFile<SeqFileHeaderCEPH, FrameFileHeaderCEPH>(file, *(SeqFileHeaderCEPH*)ptr));
                }
                else if (UEGSeqType::CT == ptr->eType)
                {
                    seqFiles.push_back(new NewSeqFile<SeqFileHeaderCT, FrameFileHeaderCT>(file, *ptr));
                }
                //LOG(INFO) << "解析SEQ文件: " << file << "成功";
            }
            else
            {
                //LOG(INFO) << "解析SEQ文件: " << file << "失败，文件真实大小和head中记录的大小不一致";
                return false;
            }
        }
        else
        {
            //LOG(INFO) << "解析SEQ文件: " << file << "失败，读取文件内容失败";
            return false;
        }
    }
    else
    {
        //LOG(INFO) << "解析SEQ文件: " << file << "失败，无法打开文件";
        return false;
    }

    return true;
}
