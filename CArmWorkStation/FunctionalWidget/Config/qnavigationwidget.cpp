#include "qnavigationwidget.h"
#include <QPainter>
#include <QDebug>

QNavigationWidget::QNavigationWidget(QWidget *parent) : QWidget(parent)
{
    backgroundColor = QColor(20, 25, 28, 255);
    selectedColor = QColor(255, 255, 255, 25);
    rowHeight = 42;
    currentIndex = 0;

    setMouseTracking(true);
    setFixedWidth(256);
}

QNavigationWidget::~QNavigationWidget()
{
}

void QNavigationWidget::addItem(const QString &title)
{
    listItems << title;

    update();
}

void QNavigationWidget::setWidth(const int &width)
{
    setFixedWidth(width);
}

void QNavigationWidget::setBackgroundColor(const QColor &color)
{
    backgroundColor = color;

    update();
}

void QNavigationWidget::setSelectColor(const QColor &color)
{
    selectedColor = color;

    update();
}

void QNavigationWidget::setRowHeight(const int &height)
{
    rowHeight = height;

    update();
}

void QNavigationWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw background color.
    painter.setPen(Qt::NoPen);
    painter.setBrush(backgroundColor);
    painter.drawRect(rect());
    QFont selectItemFont("Microsoft YaHei");
    selectItemFont.setPixelSize(16);
    QFont defaultItemFont("Microsoft YaHei");
    defaultItemFont.setPixelSize(16);
    // Draw Items
    int count = 0;
    for (const QString &str : listItems) {
        painter.setPen(Qt::NoPen);
        QPainterPath itemPath;
        itemPath.addRect(QRect(0, count * rowHeight, width(), rowHeight));

        if (currentIndex == count) {
            painter.setBrush(QColor("#0091FF"));
            painter.drawRect(QRect(26, count * rowHeight + 13, 4, 16));

            painter.setPen("#FFFFFF");
            painter.setFont(selectItemFont);
            painter.fillPath(itemPath, selectedColor);
        }else {
            painter.setPen("#B4C4D9");
            painter.setFont(defaultItemFont);
            painter.fillPath(itemPath, backgroundColor);
        }

        painter.drawText(QRect(40, count * rowHeight, width()-40, rowHeight), Qt::AlignVCenter | Qt::AlignLeft, str);

        ++count;
    }
}

void QNavigationWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (e->y() / rowHeight < listItems.count()) {
        // qDebug() << e->y() / rowHeight;
    }
}

void QNavigationWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->y() / rowHeight < listItems.count()) {
        currentIndex = e->y() / rowHeight;

        emit currentItemChanged(currentIndex);

        update();
    }
}

void QNavigationWidget::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
}
