#pragma once

#include <memory>

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class ControlSource;
class DataSource;

using std::unique_ptr;

class Detector : public QObject
{
    Q_OBJECT

public:
    Detector(ControlSource* c, DataSource* d);
    ~Detector();

signals:
    void displayMessage(const QString& info) const;
    void aFrameAcq(const int index) const;
    void frameAcqFinished() const;

public slots :
    // ControlSource连接此槽函数，通知detector去采集数据
    void acquireData() const;
    // ControlSource根据情况可使用此slot切换detector的数据来源
    void setDataSource(DataSource* src);

private slots:
    // 当有上位机连接到此探测器
    void onNewConnection();

private:
    void OnCmdRecv();
    void SendDetectorInfo() const;

private:
    QTcpServer                  m_tcpServer;            // 用于监听上位机连接的tcp服务器
    unique_ptr<ControlSource>   m_controlSource;        // 控制detector的对象
    unique_ptr<DataSource>      m_dataSource;           // 探测器的数据来源
    unique_ptr<QTcpSocket>      m_masterSocket;         // 连接到上位机的socket
};