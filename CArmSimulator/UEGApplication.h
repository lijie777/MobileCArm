#pragma once


#include <QtWidgets/QApplication>

class UEGApplication : public QApplication
{
    Q_OBJECT

public:
    UEGApplication(int &argc, char **argv);
    ~UEGApplication();

    // 初始化应用程序
    bool init() const;

signals:
    void displayMessage(const QString& info) const;

private:
    // 初始化log功能
    bool initLog() const;
};