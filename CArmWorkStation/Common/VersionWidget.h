#pragma once

#include <QtWidgets/QFrame>
#include "ui_VersionWidget.h"
class VersionWidget : public QFrame
{
    Q_OBJECT

public:
    VersionWidget(QWidget *parent = 0);
    ~VersionWidget();

private:
    Ui::VersionWidget ui;
};


