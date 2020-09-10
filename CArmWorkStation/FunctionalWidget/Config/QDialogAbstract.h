#ifndef QDIALOGABSTRACT_H
#define QDIALOGABSTRACT_H

#include <QDialog>
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
class QDialogAbstract : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogAbstract(QWidget *parent = 0);

signals:

public slots:
};

#endif // QDIALOGABSTRACT_H
