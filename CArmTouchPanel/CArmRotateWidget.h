#pragma once

#include <QtWidgets/QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <math.h>


class CArmRotateWidget : public QWidget
{
	Q_OBJECT
public:
    CArmRotateWidget(QWidget *parent = Q_NULLPTR);

protected:
	void paintEvent(QPaintEvent *event);

	void mousePressEvent(QMouseEvent *event); 

	void mouseMoveEvent(QMouseEvent *event);

signals:
    void sendCurrentPos(float);
private:

	int	m_mouseCurrentPos;

	bool m_bLeftPress;
};
