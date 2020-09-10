#ifndef QNAVIGATIONVIEW_H
#define QNAVIGATIONVIEW_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QSize>
#define UNOVERWRITE() virtual void draw(QPainter *){}    \
                      virtual void press(QMouseEvent *){}

class QNavigationAbstract;

class QNavDelegateAbstract{

public:
    QNavDelegateAbstract(QNavigationAbstract *nav){
        this->nav = nav;
    }
    virtual ~QNavDelegateAbstract(){}

    virtual void draw(QPainter *) = 0;
    virtual void press(QMouseEvent *) = 0;

public:
    QNavigationAbstract *nav;
};

class QNavHDelegate: QNavDelegateAbstract{
public:
    //UNOVERWRITE()
    QNavHDelegate(QNavigationAbstract *nav):QNavDelegateAbstract(nav){}
    virtual ~QNavHDelegate(){}

    virtual void draw(QPainter *);
    virtual void press(QMouseEvent *);
};

class QNavVDelegate: QNavDelegateAbstract{
public:
    UNOVERWRITE()
    QNavVDelegate(QNavigationAbstract *nav):QNavDelegateAbstract(nav){}
    virtual ~QNavVDelegate(){}
};

class QNavVComplexDelegate: QNavDelegateAbstract{
public:
    //UNOVERWRITE()
    QNavVComplexDelegate(QNavigationAbstract *nav):QNavDelegateAbstract(nav){}
    virtual ~QNavVComplexDelegate(){}
    virtual void draw(QPainter *);
    virtual void press(QMouseEvent *);
private:

};

class QNavigationAbstract: public QWidget {
    Q_OBJECT
public:
    explicit QNavigationAbstract(QWidget *parent = 0);
    virtual ~QNavigationAbstract();

    void addItem(const QString &title);
    void setItemSize(QSize itemSize);
    void setDelegate(QNavDelegateAbstract *);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

public:
    QList<QString> listItems;
    int currentIndex;
    QSize itemSize;
    QNavDelegateAbstract *delegate;
    QPainter *painter;

signals:
    void currentItemChanged(const int &index);
};

class QNavVComplexView : public QNavigationAbstract
{
    Q_OBJECT
public:
    explicit QNavVComplexView(QWidget *parent = 0);
    virtual ~QNavVComplexView();
};

class QNavHView : public QNavigationAbstract
{
    Q_OBJECT
public:
    explicit QNavHView(QWidget *parent = 0);
    virtual ~QNavHView();
};

#endif // QNAVIGATIONVIEW_H
