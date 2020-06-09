#include "CArmDevice.h"
#include "CArmWidget.h"
#include "Detector.h"
#include "DataSource.h"
#include "CArmSlave.h"

CArmDevice::CArmDevice()
    : m_detector(new Detector(SlavePC(), nullptr))
{
    connect(SlavePC(), &ControlSource::dataSourceChanged, m_detector, &Detector::setDataSource);
    connect(SlavePC(), &ControlSource::acquireData, m_detector, &Detector::acquireData);
    connect(m_detector, &Detector::aFrameAcq, SlavePC(), &CArmSlave::OnAFrameAcq);
    connect(m_detector, &Detector::frameAcqFinished, SlavePC(), &CArmSlave::OnFrameAcqFinished);

    connect(SlavePC(), &CArmSlave::displayMessage, Widget(), &CArmWidget::newInfoReady);
    connect(m_detector, &Detector::displayMessage, Widget(), &CArmWidget::newInfoReady);
    connect(Widget(), &CArmWidget::SendMsg, SlavePC(), &CArmSlave::SendMsg);
    connect(this, &CArmDevice::displayMessage, Widget(), &CArmWidget::newInfoReady);

    // 复位功能
    auto ResetAll = [this]
    {
        Widget()->Reset();
        SlavePC()->Reset();
    };
    connect(Widget(), &CArmWidget::ResetDevSig, [ResetAll] { ResetAll(); });
    connect(SlavePC(), &CArmSlave::ResetDevSig, [ResetAll] { ResetAll(); });

    emit displayMessage("[下位机端口号]：8888");
    emit displayMessage("[探测器端口号]：6666");
}

CArmDevice::~CArmDevice()
{

}

CArmWidget* CArmDevice::Widget()
{
    return CArmWidget::instance();
}

CArmSlave* CArmDevice::SlavePC()
{
    return CArmSlave::instance();
}

bool CArmDevice::Init()
{
    return Widget()->Init();
}
