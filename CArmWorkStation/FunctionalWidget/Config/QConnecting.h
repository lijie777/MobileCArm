#ifndef QCONNECTING_H
#define QCONNECTING_H

#include <QWidget>

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QMap>

class QConnecting;
struct Location
{
public:
    explicit Location(float _x,float _y){x=_x;y=_y;}
    float x;
    float y;
};

class BehaviorAbstract {
public:
	BehaviorAbstract(QConnecting *connectPtr):connect_(connectPtr){}
	virtual ~BehaviorAbstract() {}
	virtual void caculate() = 0;
	virtual void paint(QPainter &) = 0;
protected:
	QConnecting *connect_;
};

class LoadingBehavior : BehaviorAbstract {
public:
	LoadingBehavior(QConnecting *connectPtr):BehaviorAbstract(connectPtr){}
	virtual ~LoadingBehavior(){}
	virtual void caculate();
    virtual void paint(QPainter &);
};

class NetConnectBehavior : BehaviorAbstract {
public:
	NetConnectBehavior(QConnecting *connectPtr) :BehaviorAbstract(connectPtr) {}
	virtual ~NetConnectBehavior() {}
	virtual void caculate();
    virtual void paint(QPainter &);
};

class QConnecting : public QWidget
{
    Q_OBJECT
public:
    explicit QConnecting(QWidget *parent = 0);
    virtual ~QConnecting();

    void setDotCount(int);
    void setDotColor(const QColor&);
    void start();
    void setMaxDiameter(float);
    void setMinDiameter(float);

    virtual void setDelegate(BehaviorAbstract*);

//private:
    int _index;
    QColor _dotColor;
    int _count;
    float _minDiameter;
    float _maxDiameter;
    int _i;
    int _interval;
    QTimer timer;
    float _squareWidth;
    float _centerDistance;
    QList<float> radiiList;
    QList<Location> locationList;

	BehaviorAbstract *delegate;

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
signals:

public slots:

    void refresh();
};

class QNetConnecting: public QConnecting{
public:
    QNetConnecting(QWidget *parent = 0);
    virtual ~QNetConnecting();
};

class QLoading: public QConnecting{
public:
    QLoading(QWidget *parent = 0);
    virtual ~QLoading();

};



#endif // QCONNECTING_H
