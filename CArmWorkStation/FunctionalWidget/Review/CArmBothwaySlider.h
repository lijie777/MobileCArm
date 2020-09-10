#pragma once

#include <QtWidgets/QSlider>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>


class CArmBothwaySlider : public QSlider
{
	Q_OBJECT

public:
	CArmBothwaySlider(QWidget *parent = Q_NULLPTR);


protected:
	void paintEvent(QPaintEvent *event);


};
