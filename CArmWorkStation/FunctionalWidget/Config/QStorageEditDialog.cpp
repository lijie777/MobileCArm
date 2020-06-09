#include "QStorageEditDialog.h"
#include "ui_QStorageServiceAddDialog.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

QStorageEditDialog::QStorageEditDialog(QWidget *parent) : QStorageServiceAddDialog(parent)
{
	ui->dialogTitle->setText("编辑存储服务");
}
