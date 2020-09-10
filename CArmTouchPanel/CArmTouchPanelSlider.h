#pragma once
#include <QPainter>
#include <QtWidgets/QSlider>
#include <QMouseEvent>
#include <QPen>
#include <QBrush>
class TouchPanelSlider : public QSlider
{
    Q_OBJECT
public:
    TouchPanelSlider(QWidget *parent = Q_NULLPTR);


protected:
    void paintEvent(QPaintEvent *event);

};