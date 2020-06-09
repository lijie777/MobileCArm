#include "QNavigationView.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
void QNavVComplexDelegate::draw(QPainter *painter)
{

    QColor backgroundColor = QColor("#1F2327");
    QColor selectedColor = QColor(0, 0, 0, 127);
    painter->setRenderHint(QPainter::Antialiasing, true);
    // Draw background color.
    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor);
    painter->drawRect(nav->rect());

    QFont selectItemFont("Microsoft YaHei");
    selectItemFont.setPixelSize(16);
    QFont defaultItemFont("Microsoft YaHei");
    defaultItemFont.setPixelSize(16);
    QFont defaultTextFont("Microsoft YaHei");
    defaultTextFont.setPixelSize(14);
    // Draw Items
    int count = 0;
    for (const QString &str : nav->listItems) {
        painter->setPen(Qt::NoPen);
        QPainterPath itemPath;
        itemPath.addRect(QRect(0, count * nav->itemSize.height(), nav->itemSize.width(), nav->itemSize.height()));

        if (nav->currentIndex == count) {
            painter->setPen(QColor("#B4C4D9"));
            painter->setFont(defaultTextFont);
            painter->drawText(QRect(57, count * nav->itemSize.height()+48, 28, 16), Qt::AlignVCenter | Qt::AlignHCenter, QString("默认"));
            //painter->drawRect(QRect(26, count * nav->itemSize.height() + 13, 4, 16));

            painter->setPen("#FFFFFF");
            painter->setFont(selectItemFont);
            painter->fillPath(itemPath, selectedColor);
        }else {
            painter->setPen("#B4C4D9");
            painter->setFont(defaultItemFont);
            painter->fillPath(itemPath, backgroundColor);
        }

        painter->drawText(QRect(0, count * nav->itemSize.height(), nav->itemSize.width(), nav->itemSize.height()), Qt::AlignVCenter | Qt::AlignHCenter, str);

        ++count;
    }
}

void QNavVComplexDelegate::press(QMouseEvent *e)
{
        if (e->y() / nav->itemSize.height() < nav->listItems.count()) {
            nav->currentIndex = e->y() / nav->itemSize.height();

            emit nav->currentItemChanged(nav->currentIndex);

            nav->update();
        }
}

void QNavHDelegate::draw(QPainter *painter)
{
    QColor backgroundColor = QColor(0, 0, 0, 26);
    QColor selectedColor = QColor(46, 124, 211,255);
    painter->setRenderHint(QPainter::Antialiasing, true);
    // Draw background color.
    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor);
    painter->drawRect(nav->rect());

    QFont selectItemFont("Microsoft YaHei");
    selectItemFont.setPixelSize(16);
    QFont defaultItemFont("Microsoft YaHei");
    defaultItemFont.setPixelSize(16);
    // Draw Items
    int count = 0;
    for (const QString &str : nav->listItems) {
        painter->setPen(Qt::NoPen);
        QPainterPath itemPath;
        itemPath.addRect(QRect(count * nav->itemSize.width(), 0, nav->itemSize.width(), nav->itemSize.height()));

        if (nav->currentIndex == count) {
            //painter->setBrush(QColor("#0091FF"));
            //painter->drawRect(QRect(26, count * nav->itemSize.height() + 13, 4, 16));

            painter->setPen("#FFFFFF");
            painter->setFont(selectItemFont);
            painter->fillPath(itemPath, selectedColor);
        }else {
            painter->setPen("#B4C4D9");
            painter->setFont(defaultItemFont);
            painter->fillPath(itemPath, backgroundColor);
        }

        painter->drawText(QRect(count * nav->itemSize.width(), 0, nav->itemSize.width(), nav->itemSize.height()), Qt::AlignVCenter | Qt::AlignHCenter, str);

        ++count;
    }
}

void QNavHDelegate::press(QMouseEvent *e)
{
    if (e->x() / nav->itemSize.width() < nav->listItems.count()) {
        nav->currentIndex = e->x() / nav->itemSize.width();

        emit nav->currentItemChanged(nav->currentIndex);

        nav->update();
    }
}

QNavigationAbstract::QNavigationAbstract(QWidget *parent) :QWidget(parent)
{
    delegate = NULL;
}

QNavigationAbstract::~QNavigationAbstract()
{
    if(!delegate)
        delete delegate;
}

void QNavigationAbstract::addItem(const QString &title)
{
    listItems << title;

    update();
}

void QNavigationAbstract::setItemSize(QSize itemSize)
{
    this->itemSize = itemSize;
}

void QNavigationAbstract::setDelegate(QNavDelegateAbstract *delegate)
{
    this->delegate = delegate;
    update();
}

void QNavigationAbstract::paintEvent(QPaintEvent *)
{
    if(!delegate)
        return;
    QPainter painter(this);
    delegate->draw(&painter);
}

void QNavigationAbstract::mouseMoveEvent(QMouseEvent *e)
{
    //if (e->y() / rowHeight < listItems.count()) {
        // qDebug() << e->y() / rowHeight;
    //}
}

void QNavigationAbstract::mousePressEvent(QMouseEvent *e)
{
    if(!delegate)
        return;
    delegate->press(e);
}

void QNavigationAbstract::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
}

QNavVComplexView::QNavVComplexView(QWidget *parent) : QNavigationAbstract(parent)
{
    currentIndex = 0;
    setItemSize(QSize(142, 70));
    setMouseTracking(true);
    delegate = NULL;
    setDelegate((QNavDelegateAbstract *)new QNavVComplexDelegate(this));
}

QNavVComplexView::~QNavVComplexView()
{

}

QNavHView::QNavHView(QWidget *parent): QNavigationAbstract(parent)
{
    currentIndex = 0;
    setItemSize(QSize(120, 34));
    setMouseTracking(true);
    delegate = NULL;
    setDelegate((QNavDelegateAbstract *)new QNavHDelegate(this));
}

QNavHView::~QNavHView()
{

}
