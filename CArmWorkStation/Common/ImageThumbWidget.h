#pragma once

#include <QtWidgets/QFrame>
#include "ui_ImageThumbWidget.h"

class ImageThumbWidget : public QFrame
{
    Q_OBJECT

public:
    ImageThumbWidget(QWidget *parent = 0);
    ~ImageThumbWidget();

private:
    Ui::ImageThumbWidget ui;
};


