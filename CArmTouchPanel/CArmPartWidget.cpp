#include "CArmPartWidget.h"
#include <QPixmap>
#include <QString>

CArmPartWidget::CArmPartWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    this->installEventFilter(this);
}

CArmPartWidget::~CArmPartWidget()
{
}

bool CArmPartWidget::eventFilter(QObject *Object, QEvent *event)
{
    if (Object == this && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *buttonPress = static_cast<QMouseEvent*>(event);
        if (buttonPress->button() == Qt::LeftButton)
        {
            // 显示对应的模式
            emit bodyPartSignal();
        }
    }
    return true;
}


void CArmPartWidget::setDescription(QString string)
{
    ui.descriptionLbe->setText(string);
}

void CArmPartWidget::setPicture(char* path)
{
    ui.pictureLbe->setPixmap(QPixmap(QString::fromUtf8(path)));

}

void CArmPartWidget::styleSet(void)
{
    ui.descriptionLbe->setStyleSheet("border:0px;}");
    ui.pictureLbe->setStyleSheet("border:0px;}");
}
