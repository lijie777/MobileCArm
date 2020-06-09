#include "CArmRotateWidget.h"

CArmRotateWidget::CArmRotateWidget(QWidget *parent)
	: QWidget(parent)
	, m_mouseCurrentPos(245)
	, m_bLeftPress(false)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
	this->setStyleSheet("background-color:rgb(0,0,0,0.29)");
}

void CArmRotateWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	/*反锯齿*/
	painter.setRenderHint(QPainter::Antialiasing, true);
	
    /*绘制滑块槽*/
    QPen pen(QColor(86, 101, 118));
	QBrush lineBrush(QColor(66, 80, 96), Qt::SolidPattern);
	pen.setWidth(4);
	painter.setBrush(lineBrush);
	painter.setPen(pen);
	painter.drawLine(0, 80, 500, 80);

    /*绘制滑块*/
	QPen rectPen(QColor(255, 255, 255, 255));
	QBrush rectBrush(Qt::white, Qt::SolidPattern);
	painter.setBrush(rectBrush);
	painter.setPen(rectPen);
	painter.drawRect(m_mouseCurrentPos, 69, 22, 22);

    /*绘制划过去线颜色*/
	if (m_bLeftPress == true)
	{
		QPen pressColor(QColor(0, 145, 255));
		QBrush pressBrush(QColor(0, 145, 255), Qt::SolidPattern);
		pressColor.setWidth(4);

		painter.setBrush(pressBrush);
		painter.setPen(pressColor);
		if (m_mouseCurrentPos >= 260)
		{
			painter.drawLine(250, 80, m_mouseCurrentPos - 3, 80);
		}
		else if(m_mouseCurrentPos <= 240)
		{
			painter.drawLine(250, 80, m_mouseCurrentPos + 25, 80);
		}
	}

	QWidget::paintEvent(event);
}

void CArmRotateWidget::mouseMoveEvent(QMouseEvent *event)
{
    /*不让滑块滑出窗口*/
	if (event->pos().x() >= 480 || event->pos().x() < 0)
		return;

    

	if (m_bLeftPress == true)
	{
        /*发送当前值*/
		m_mouseCurrentPos = event->pos().x();
        if (event->pos().x() <= 480 && event->pos().x() >= 250)
        {
            emit sendCurrentPos(round(event->pos().x()*0.75 + 1) - 180);
        }
        else
        {
            emit sendCurrentPos(round(event->pos().x()*0.72) - 180);
        }
		update();
	}

	QWidget::mouseMoveEvent(event);
}

void CArmRotateWidget::mousePressEvent(QMouseEvent* event)
{
    /*判断是否点击到滑块上面*/
	if (event->pos().x() >= m_mouseCurrentPos && event->pos().x() <= m_mouseCurrentPos + 20)
	{
		m_bLeftPress = true;
       
	}
	else
	{
		m_bLeftPress = false;
	}
	QWidget::mousePressEvent(event);
}