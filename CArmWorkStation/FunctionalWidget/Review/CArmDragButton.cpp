#include "CArmDragButton.h"



CArmDragButton::CArmDragButton(QWidget* parent)
{
    this->setStyleSheet("QPushButton\
    {background:transparent;\
    border-radius:4px 0px 0px 4px;\
    border:none;\
    image:url(:/FunctionalWidget/FunctionalWidget/Review/Resource/hisnormal.png);}\
    QPushButton:disabled{\
    image:url(:/FunctionalWidget/FunctionalWidget/Review/Resource/hisdisable.png);}");
    this->setFixedSize(36, 66);
}


CArmDragButton::~CArmDragButton()
{
}
