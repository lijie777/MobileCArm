#pragma once

#include <QtWidgets/QFrame>
#include "ui_ExportFormatWidget.h"

class ExportFormatWidget : public QFrame
{
    Q_OBJECT

public:
    ExportFormatWidget(QWidget *parent = 0);
    ~ExportFormatWidget();

private:
    Ui::ExportFormatWidget ui;
};


