#include "Dialog.h"
#include "QWorkListAddDialog.h"
#include "QDicomAddDialog.h"
#include "QStorageServiceAddDialog.h"
#include "QMppsAddDialog.h"
#include "QQueryRetrieveAddDialog.h"
#include "QConnectTestDialog.h"
#include "QWorkListEditDialog.h"
#include "QDiconPrintEditDialog.h"
#include "QStorageEditDialog.h"
#include "QQREditDialog.h"
#include "QMppsEditDialog.h"
#include "UserAddDialog.h"

Dialog::Dialog(QObject *parent) : QObject(parent)
{
    dialog = Q_NULLPTR;
}
Dialog::~Dialog(){
    realseDialog();
}

void Dialog::realseDialog(){
    if(!dialog){
        delete dialog;
        dialog = Q_NULLPTR;
    }
}

void Dialog::setDialogParent(QWidget *dialogParent)
{
    this->dialogParent = dialogParent;
    connect(this, SIGNAL(showDialog(DialogType)), this ,SLOT(showDialogSlot(DialogType)));
}

void Dialog::showDialogSlot(Dialog::DialogType dialogType)
{
    //this->setAttribute(Qt::WA_DeleteOnClose,true);模态Dialog close()会自动释放
    //realseDialog();
    switch (dialogType) {
    case WorkListAdd:{
        dialog = new QWorkListAddDialog(dialogParent);
        dialog->show();
    }
        break;
    case WorkListEdit:{
        dialog = new QWorkListEditDialog(dialogParent);
        dialog->show();
    }
        break;
    case DicomPrintAdd:{
        dialog = new QDicomAddDialog(dialogParent);
        dialog->show();
    }
        break;
    case DicomPrintEdit:{
        dialog = new QDiconPrintEditDialog(dialogParent);
        dialog->show();
    }
        break;
    case StorageServiceAdd:{
        dialog = new QStorageServiceAddDialog(dialogParent);
        dialog->show();
    }
        break;
    case StorageServiceEdit:{
        dialog = new QStorageEditDialog(dialogParent);
        dialog->show();
    }
        break;
    case MppsServerAdd:{
        dialog = new QMppsAddDialog(dialogParent);
        dialog->show();
    }
        break;
    case MppsSerVerEdit:{
        dialog = new QMppsEditDialog(dialogParent);
        dialog->show();
    }
        break;
    case QueryRetrieveAdd:{
        dialog = new QQueryRetrieveAddDialog(dialogParent);
        dialog->show();
    }
        break;
    case QueryRetrieveEdit:{
        dialog = new QQREditDialog(dialogParent);
        dialog->show();
    }
        break;
    case ConnectTest:{
        dialog = new QConnectTestDialog(dialogParent);
        dialog->show();
    }
        break;
    case UserAdd: {
        dialog = new UserAddDialog(dialogParent);
        dialog->show();
    }
        break;
    default:
        break;
    }
}
