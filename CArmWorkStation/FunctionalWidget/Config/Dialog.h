#ifndef DIALOG_H
#define DIALOG_H

#include <QObject>
#include <QDialog>
#include "QDialogAbstract.h"
class Dialog : public QObject
{
    Q_OBJECT
public:
    enum DialogType {
        ConnectTest,
        WorkListIndex,
        WorkListAdd,
        WorkListEdit,
        DicomPrintIndex,
        DicomPrintAdd,
        DicomPrintEdit,
        StorageServiceIndex,
        StorageServiceAdd,
        StorageServiceEdit,
        MppsServerIndex,
        MppsServerAdd,
        MppsSerVerEdit,
        QueryRetrieveIndex,
        QueryRetrieveAdd,
        QueryRetrieveEdit,
        UserAdd
    };

    explicit Dialog(QObject *parent = 0);
    virtual ~Dialog();
    void realseDialog();

    void setDialogParent(QWidget *dialogParent);

signals:
    void showDialog(DialogType);

public slots:
    void showDialogSlot(DialogType);

private:
    QWidget *dialogParent;
    QDialogAbstract *dialog;

};

#endif // DIALOG_H
