#include "QConnecting.h"
#include "qmath.h"
#include <QDebug>

QConnecting::QConnecting(QWidget *parent) : QWidget(parent),_i(0),_interval(250),_index(0)
{
    delegate =NULL;
}

QConnecting::~QConnecting()
{
    if(!delegate){
        delete delegate;
        delegate =NULL;
    }
}


void QConnecting::setDotCount(int count)
{
    _count=count;
}


void QConnecting::setDotColor(const QColor & color)
{
    _dotColor=color;
}


void QConnecting::start()
{
    timer.setInterval(_interval);
    timer.start();
}


void QConnecting::setMaxDiameter(float max)
{
    _maxDiameter=max;
}


void QConnecting::setMinDiameter(float min)
{
    _minDiameter=min;
}

void QConnecting::setDelegate(BehaviorAbstract* behavior)
{
    delegate = behavior;
}

void QConnecting::refresh()
{
    repaint();
}

void QConnecting::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    delegate->caculate();
}

void QConnecting::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //painter.setPen(_dotColor);
	//painter.setBrush(_dotColor);

    delegate->paint(painter);
	
}

void LoadingBehavior::caculate()
{
    connect_->_squareWidth=qMin(connect_->width(),connect_->height());
    float half=connect_->_squareWidth/2;
    connect_->_centerDistance=half-connect_->_maxDiameter/2-1;

    float gap=(connect_->_maxDiameter-connect_->_minDiameter)/(connect_->_count-1)/2;
    float angleGap=(float)360/connect_->_count;

    connect_->locationList.clear();
    connect_->radiiList.clear();

    for(int i=0;i<connect_->_count;i++)
    {
        connect_->radiiList<<connect_->_maxDiameter/2-i*gap;
        float radian=qDegreesToRadians(-angleGap*i);
        connect_->locationList.append(Location(half+connect_->_centerDistance*qCos(radian),half-connect_->_centerDistance*qSin(radian)));
    }
}

void LoadingBehavior::paint(QPainter &painter)
{
	painter.setPen(connect_->_dotColor);
	painter.setBrush(connect_->_dotColor);
    for(int i=0;i<connect_->_count;i++)
    {
        painter.setPen(connect_->_dotColor);
        float radii=connect_->radiiList.at((connect_->_index+connect_->_count-i)%connect_->_count);

        painter.drawEllipse(QPointF(connect_->locationList.at(i).x,connect_->locationList.at(i).y),radii,radii);

    }
    connect_->_index++;
}

void NetConnectBehavior::caculate()
{
    connect_->_squareWidth=qMin(connect_->width(),connect_->height());
    float half=connect_->_squareWidth/2;
    connect_->_centerDistance=half-connect_->_maxDiameter/2-1;

    float gap=(connect_->_maxDiameter-connect_->_minDiameter)/(connect_->_count-1)/2;
    //float angleGap=(float)360/connect_->_count;

    connect_->locationList.clear();
    connect_->radiiList.clear();

    for(int i=0;i<connect_->_count;i++)
    {
        connect_->radiiList<<connect_->_maxDiameter/2-i*gap;
        connect_->locationList.append(Location(connect_->_maxDiameter/2.0+(connect_->_squareWidth-connect_->_maxDiameter)/(float)(connect_->_count-1)*i,connect_->_squareWidth/2));
    }
}

void NetConnectBehavior::paint(QPainter &painter)
{
    for(int i=0;i<connect_->_count;i++)
    {
		if (i <= (connect_->_index%connect_->_count)) {
			painter.setPen(connect_->_dotColor);
			painter.setBrush(connect_->_dotColor);
		}else {
            painter.setPen(QColor(177,193,214,255*0.4));
            painter.setBrush(QColor(177,193,214,255*0.4));
		}
        float radii=connect_->radiiList.at((connect_->_index+connect_->_count-i)%connect_->_count);

        painter.drawEllipse(QPointF(connect_->locationList.at(i).x,connect_->locationList.at(i).y),radii,radii);

    }
    connect_->_index++;
}

QNetConnecting::QNetConnecting(QWidget *parent): QConnecting(parent)
{
    setDotColor(QColor(177,193,214,255));
    setDotCount(3);
    connect(&timer,&QTimer::timeout,this,&QConnecting::refresh);
    setMaxDiameter(5);
    setMinDiameter(5);
    setGeometry(100, 100, 31, 31);
	setDelegate((BehaviorAbstract*)new NetConnectBehavior(this));
}

QNetConnecting::~QNetConnecting()
{
	if (!delegate) {
		delete delegate;
		delegate = NULL;
	}
}

QLoading::QLoading(QWidget *parent): QConnecting(parent)
{
    setDotColor(QColor(177,193,214,255));
    setDotCount(3);
    connect(&timer,&QTimer::timeout,this,&QConnecting::refresh);
    setMaxDiameter(5);
    setMinDiameter(5);
    setGeometry(100, 100, 31, 31);
	setDelegate((BehaviorAbstract*)new LoadingBehavior(this));
}

QLoading::~QLoading()
{
	if (!delegate) {
		delete delegate;
		delegate = NULL;
	}
}
