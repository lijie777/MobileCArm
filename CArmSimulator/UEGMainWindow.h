#pragma once

#include <QtWidgets/QMainWindow>

class CArmDevice;

class UEGMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    UEGMainWindow(QWidget *parent = nullptr);
    bool Init();

signals:
    void displayMessage(const QString& info) const;

private:
    CArmDevice*           m_device;
};
