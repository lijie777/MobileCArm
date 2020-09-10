#include <fstream>
#include <array>

#include "DataSource.h"

enum PACKAGE_INFO
{
    TOTAL_PACKAGE_NUM = 0,
    PER_PACKAGE_SIZE,
    LAST_PACKAGE_SIZE,
    PACKAGE_INFO_SIZE
};

DataSource::DataSource(const SeqFileBase* seq)
    : m_seq(seq)
{

}

DataSource::~DataSource()
{
    delete m_seq;
}

// 将原始数据发送给指定的socket
void DataSource::sendFrameData(QTcpSocket* socket) const
{
    ON_SCOPE_EXIT([this]
    {
        frameSendFinished();
    });

    sendFileData(socket);
}

// 将数据发送到指定的socket
void DataSource::sendData(QTcpSocket* socket) const
{
    if (!m_seq->valid() || !UegNetwork::isConnected(socket))
    {
        return;
    }

    // 数据包信息
    if (m_seq->isNewFile())
    {
        sendFrameInfo(socket);
    }

    sendFrameData(socket);
}

bool DataSource::useUp() const
{
    return !m_seq->valid();
}

// 发送文件第一帧数据前，先将帧的信息发送给对方
// -----------------------------------
// | 总包数 | 每包大小 | 最后一包大小 |
// -----------------------------------
// 发送文件数据前，先将上面三个数据发送给对方，每个数据占4 byte
void DataSource::sendFrameInfo(QTcpSocket* socket) const
{
    const auto frameSize = m_seq->frameSize_Byte()+1;// 第一个字节表示本帧是否为丢帧
    uint32_t info[PACKAGE_INFO_SIZE]{};

    info[PER_PACKAGE_SIZE] = PER_PACKAGE_BYTE; // 一帧数据分多次发送，每次发送的大小
    const auto completePackageNum = frameSize / info[PER_PACKAGE_SIZE];    // 完整的包的个数
    const auto incompletePackageSize = frameSize % info[PER_PACKAGE_SIZE]; // 不完整的包有多少数据
    info[TOTAL_PACKAGE_NUM] = completePackageNum + (incompletePackageSize > 0 ? 1 : 0);
    info[LAST_PACKAGE_SIZE] = (incompletePackageSize > 0 ? incompletePackageSize : info[PER_PACKAGE_SIZE]);
    UegNetwork::sendData(socket, info, sizeof(info));
}

// 获取当前指向的文件中的数据
void DataSource::sendFileData(QTcpSocket* socket) const
{
    if (std::ifstream ifs{ m_seq->filename(), std::ios::binary })
    {
        const auto frameSizeByte = m_seq->frameSize_Byte();
        const auto allDataSize = frameSizeByte + 1;// 第一个字节表示本帧是否为丢帧

        std::vector<char> frame(allDataSize);
        frame[0] = m_seq->CurrentFrameLost();

        ifs.seekg(m_seq->fileOffset());
        ifs.read(&frame[1], frameSizeByte);

        typedef char(*pPackage)[PER_PACKAGE_BYTE];
        pPackage pFrame = (pPackage)(frame.data());

        signed long long size = allDataSize;
        while (size > 0)
        {
            UegNetwork::sendData(socket, pFrame, ((size < PER_PACKAGE_BYTE) ? size: PER_PACKAGE_BYTE));
            size -= PER_PACKAGE_BYTE;
            ++pFrame;
        }
    }
}

// 将文件偏移移动到下一帧
void DataSource::frameSendFinished() const
{
    m_seq->toNextFrame();
}
