#pragma once

#include <map>
#include <vector>
#include <string>

#include "common.h"

// seq文件存放目录
constexpr static char* SEQ_FILE_DIR = "\\SEQ_FILE";

namespace SeqFileParser
{
    // 遍历seq存放目录下的文件并解析
    bool parseSeq();
    // 解析指定路径的seq文件
    bool parseNewTypeSeq(const std::string& file);
    // 被解析文件是否准备好
    bool dataDirCheck();
    // 获取所有的seq文件
    const SeqFileVec_t& getSeqVec();
}