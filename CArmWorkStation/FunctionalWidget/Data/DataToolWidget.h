#pragma once

#include <QtWidgets/QFrame>
#include "ui_DataToolWidget.h"

class DataToolWidget : public QFrame
{
    Q_OBJECT

public:
    DataToolWidget(QWidget *parent = 0);
    ~DataToolWidget();

private:
    Ui::DataToolWidget ui;
};


