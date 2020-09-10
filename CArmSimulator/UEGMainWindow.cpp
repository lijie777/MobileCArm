#include <QHBoxLayout>

#include "UEGMainWindow.h"
#include "CArmDevice.h"

UEGMainWindow::UEGMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_device(new CArmDevice)
{
    resize(600, 600);
    setCentralWidget(new QWidget);

    auto hLayout = new QHBoxLayout;
    hLayout->addWidget((QWidget*)(m_device->Widget()));

    centralWidget()->setLayout(hLayout);

    connect(this, &UEGMainWindow::displayMessage, m_device, &CArmDevice::displayMessage);
}

bool UEGMainWindow::Init()
{
    return m_device->Init();
}
