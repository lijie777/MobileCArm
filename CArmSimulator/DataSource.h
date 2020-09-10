#pragma once

#include <string>

#include <QObject>
#include <QTcpSocket>

#include "common.h"
#include "SeqFile.h"

class DataSource : public QObject
{
    Q_OBJECT

public:
    DataSource(const SeqFileBase* seq);
    ~DataSource();

    // 将数据发送到指定的socket
    void sendData(QTcpSocket* socket) const;
    inline int currentIndex() const { return m_seq->frameIndex(); }
    bool useUp() const;

private:
    // 获取当前指向的文件中的数据
    void sendFileData(QTcpSocket* socket) const;
    // 将文件偏移移动到下一帧
    void frameSendFinished() const;
    // 将原始数据发送给指定的socket
    void sendFrameData(QTcpSocket* socket) const;
    /// 发送文件第一帧数据前，先将帧的信息发送给对方
    void sendFrameInfo(QTcpSocket* socket) const;

private:
    const SeqFileBase*                m_seq;           // seq文件
};