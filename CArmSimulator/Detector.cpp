#include "Detector.h"
#include "DataSource.h"
#include "ControlSource.h"
#include "common.h"
#include "IPCDefinitions.h"
#include "CArmWidget.h"

Detector::Detector(ControlSource* c, DataSource* d)
    : m_controlSource(c)
    , m_dataSource(d)
    , m_masterSocket(nullptr)
{
    connect(&m_tcpServer, &QTcpServer::newConnection, this, &Detector::onNewConnection);
    m_tcpServer.listen(QHostAddress::Any, 6666);
}

Detector::~Detector()
{

}

// ControlSource连接此槽函数，通知detector去采集数据
void Detector::acquireData() const
{
    if (m_masterSocket)
    {
        if (m_dataSource && !m_dataSource->useUp())
        {
            m_dataSource->sendData(m_masterSocket.get());
            emit aFrameAcq(m_dataSource->currentIndex());
            if (m_dataSource->useUp())
            {
                emit frameAcqFinished();
            }
        }
        else
        {
            std::uint32_t info[3]{};
            UegNetwork::sendData(m_masterSocket.get(), &info, sizeof(info));
        }
    }
}

// ControlSource根据情况可使用此slot切换detector的数据来源
void Detector::setDataSource(DataSource* src)
{
    m_dataSource.reset(src);
}

// 当有上位机连接到此探测器
void Detector::onNewConnection()
{
    // 探测器只允许single client连接。若上个连接未关闭，则直接关闭新的连接请求
    if (m_masterSocket && (QAbstractSocket::ConnectedState == m_masterSocket->state()))
    {
        m_tcpServer.nextPendingConnection()->close();
        return;
    }

    m_masterSocket.reset(m_tcpServer.nextPendingConnection());
    emit displayMessage("[探测器] 已和上位机建立连接: Ip: " + m_masterSocket->peerAddress().toString() + ", port: " + QString::number(m_masterSocket->peerPort()));
    connect(m_masterSocket.get(), &QTcpSocket::stateChanged, [&]()
    {
        if (m_masterSocket && (QAbstractSocket::UnconnectedState == m_masterSocket->state()))
        {
            emit displayMessage("[探测器] 已和上位机断开连接.");
        }
    });
    connect(m_masterSocket.get(), &QTcpSocket::readyRead, [this] { OnCmdRecv(); });
}

void Detector::OnCmdRecv()
{
    const auto cmd = m_masterSocket->readAll();

    if (cmd == CMD::GET_SIMUL_IMG_DATA.data())
    {
        acquireData();
    }
    else if (cmd == CMD::GET_SIMUL_DETECTOR_MODEL.data())
    {
        SendDetectorInfo();
    }
}

void Detector::SendDetectorInfo() const
{
    const auto detector = CArmWidget::instance()->GetDetectorInfo();
    if (detector)
    {
        const auto info = detector->vendor + "," + detector->model;
        UegNetwork::sendData(m_masterSocket.get(), info.data(), info.size());
    }
    else
    {
        emit displayMessage("未能获取探测器信息");
    }
}
