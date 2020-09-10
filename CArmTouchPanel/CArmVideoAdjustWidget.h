#pragma once

#include <QWidget>
#include "ui_CArmVideoAdjustWidget.h"

class CArmVideoAdjustWidget : public QWidget, public Ui::videoWidget
{
    Q_OBJECT
public:
    CArmVideoAdjustWidget(QWidget *parent = Q_NULLPTR);
    ~CArmVideoAdjustWidget();

private slots:
    void playOrPauseVideo_slot();

signals:
    void playOrPauseVideo(bool);


private:
    bool    m_bPlayOrPause;
};
