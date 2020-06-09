#ifndef QSERVERTAB_H
#define QSERVERTAB_H

#include <QWidget>
#include "Dialog.h"
#include "singleton.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
namespace Ui {
class QServerTab;
}

class QServerTab : public QWidget
{
    Q_OBJECT

public:
    explicit QServerTab(QWidget *parent = 0);
    ~QServerTab();
protected:
    void init();
protected:
    Ui::QServerTab *ui;
    int dialogIndex;

};

#endif // QSERVERTAB_H
