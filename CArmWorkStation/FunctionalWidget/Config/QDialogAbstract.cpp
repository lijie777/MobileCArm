#include "QDialogAbstract.h"

QDialogAbstract::QDialogAbstract(QWidget *parent) : QDialog(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose,true);
}
