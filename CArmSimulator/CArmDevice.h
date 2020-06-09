#pragma once

#include <QObject>

class CArmSlave;
class Detector;
class CArmWidget;

class CArmDevice : public QObject
{
    Q_OBJECT

public:
    CArmDevice();
    ~CArmDevice();

    // 设备模拟器的界面
    CArmWidget* Widget();
    //设备模拟器的控制源
    CArmSlave* SlavePC();
    bool Init();

signals:
    void displayMessage(const QString& info) const;

private:
    Detector*           m_detector;         // 探测器
};