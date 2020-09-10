#include "UserAddDialog.h"

UserAddDialog::UserAddDialog(QWidget *parent):QDialogAbstract(parent)
{
    ui.setupUi(this);

    setModal(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    connect(ui.closeBtn, &QPushButton::clicked, this, &UserAddDialog::closeBtnSlot);
}
UserAddDialog::~UserAddDialog()
{

}

void UserAddDialog::closeBtnSlot()
{
    this->close();
}
void UserAddDialog::certatinBtnSlot()
{
    // 用户信息写入数据库
    this->close();
}
