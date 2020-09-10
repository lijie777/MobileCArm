#ifndef QCFGTIMEZONEPAGE_H
#define QCFGTIMEZONEPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"
class QCfgTimeZonePage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgTimeZonePage(QWidget *parent = 0);
    virtual ~QCfgTimeZonePage();
    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
signals:

public slots:
};

#endif // QCFGTIMEZONEPAGE_H
