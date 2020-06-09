#include "SuspendedScrollbarListWidget.h"



#define _MARGIN_SCROLLBAR 2


/**
* @brief 悬浮滚动条效果的列表控件
*/
SuspendedScrollbarListWidget::SuspendedScrollbarListWidget(QWidget* widget)
    : QListWidget(widget)
    , m_pHorizScrollBar(nullptr)
    , m_pVertScrollBar(nullptr)
{
    initUI();
    initLogic();
}

SuspendedScrollbarListWidget::~SuspendedScrollbarListWidget()
{
    
}

void SuspendedScrollbarListWidget::setHorizontalScrollbar(QScrollBar * horizbar)
{

    disconnect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateHorizontalValue(int)));

    disconnect(horizontalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(updateHorizontalRange(int, int)));

    if (m_pHorizScrollBar != nullptr)
    {
        disconnect(m_pHorizScrollBar, SIGNAL(valueChanged(int)), this, SLOT(trackHorizontalValue(int)));
    }


    m_pHorizScrollBar = horizbar;

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateHorizontalValue(int)));

    connect(horizontalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(updateHorizontalRange(int, int)));

    if (m_pHorizScrollBar != nullptr)
    {
        connect(m_pHorizScrollBar, SIGNAL(valueChanged(int)), this, SLOT(trackHorizontalValue(int)));
        m_pHorizScrollBar->hide();
    }

}

void SuspendedScrollbarListWidget::setVerticalScrollbar(QScrollBar * vertbar)
{
    disconnect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateVerticalValue(int)));

    disconnect(verticalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(updateVerticalRange(int, int)));

    if (m_pVertScrollBar != nullptr)
    {
        disconnect(m_pVertScrollBar, SIGNAL(valueChanged(int)), this, SLOT(trackVerticalValue(int)));
    }

    m_pVertScrollBar = vertbar;

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateVerticalValue(int)));

    connect(verticalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(updateVerticalRange(int, int)));

    if (m_pVertScrollBar != nullptr)
    {
        connect(m_pVertScrollBar, SIGNAL(valueChanged(int)), this, SLOT(trackVerticalValue(int)));
        m_pVertScrollBar->hide();
    }

}

void SuspendedScrollbarListWidget::updateHorizontalValue(int i)
{
    if (m_pHorizScrollBar != nullptr)
    {
        m_pHorizScrollBar->setValue(i);
    }
}

void SuspendedScrollbarListWidget::updateVerticalValue(int i)
{
    if (m_pVertScrollBar != nullptr)
    {
        m_pVertScrollBar->setValue(i);
    }
}

void SuspendedScrollbarListWidget::updateHorizontalRange(int min, int max)
{
    if (m_pHorizScrollBar != nullptr)
    {
        m_pHorizScrollBar->setRange(min, max);

        if (max <= 0) m_pHorizScrollBar->hide();
    }
}

void SuspendedScrollbarListWidget::updateVerticalRange(int min, int max)
{
    if (m_pVertScrollBar != nullptr)
    {
        m_pVertScrollBar->setRange(min, max);

        if (max <= 0) m_pVertScrollBar->hide();
    }
}

void SuspendedScrollbarListWidget::trackHorizontalValue(int i)
{
    horizontalScrollBar()->setValue(i);
}

void SuspendedScrollbarListWidget::trackVerticalValue(int i)
{
    verticalScrollBar()->setValue(i);
}

void SuspendedScrollbarListWidget::initUI()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void SuspendedScrollbarListWidget::initLogic()
{

}

void SuspendedScrollbarListWidget::resizeEvent(QResizeEvent * e)
{
    if (m_pVertScrollBar != nullptr)
    {
        int iX = this->width() - m_pVertScrollBar->width() - _MARGIN_SCROLLBAR;
        m_pVertScrollBar->setGeometry(iX, 1, m_pVertScrollBar->width(), this->height() - _MARGIN_SCROLLBAR);
    }

    if (m_pHorizScrollBar != nullptr)
    {
        int iY = this->height() - m_pHorizScrollBar->height() - _MARGIN_SCROLLBAR;
        m_pHorizScrollBar->setGeometry(1, iY, this->width() - _MARGIN_SCROLLBAR, m_pHorizScrollBar->height());
    }

    return QListWidget::resizeEvent(e);
}

void SuspendedScrollbarListWidget::enterEvent(QEvent * e)
{
    if (m_pVertScrollBar != nullptr)
    {
        if (m_pVertScrollBar->maximum() > 0) 
        {
            int iX = this->width() - m_pVertScrollBar->width() - _MARGIN_SCROLLBAR;
            m_pVertScrollBar->setGeometry(iX, 1, m_pVertScrollBar->width(), this->height() - _MARGIN_SCROLLBAR);
            m_pVertScrollBar->show(); 
        }
    }

    if (m_pHorizScrollBar != nullptr)
    {
        int iY = this->height() - m_pHorizScrollBar->height() - _MARGIN_SCROLLBAR;
        m_pHorizScrollBar->setGeometry(1, iY, this->width() - _MARGIN_SCROLLBAR, m_pHorizScrollBar->height());
        if (m_pHorizScrollBar->maximum() > 0)  m_pHorizScrollBar->show();
    }

    setCursor(Qt::ArrowCursor);
    return QListWidget::enterEvent(e);
}

void SuspendedScrollbarListWidget::leaveEvent(QEvent * e)
{
    if (m_pVertScrollBar != nullptr)
    {
         m_pVertScrollBar->hide();
    }

    if (m_pHorizScrollBar != nullptr)
    {
        m_pHorizScrollBar->hide();
    }

    return QListWidget::leaveEvent(e);

}


/**
* @brief 列表控件展开折叠控制窗口
*/
ListWidgetFoldWidget::ListWidgetFoldWidget(QWidget * parent)
{
}

ListWidgetFoldWidget::~ListWidgetFoldWidget()
{
}

void ListWidgetFoldWidget::enterEvent(QEvent * e)
{
    setCursor(Qt::ArrowCursor);
    return QWidget::enterEvent(e);
}
