#ifndef QCFGIMGSAMPAGE_H
#define QCFGIMGSAMPAGE_H
#include "QConfigPageAbstract.h"
#include <QWidget>

class QCfgImgSamPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgImgSamPage(QWidget *parent = 0);
    virtual ~QCfgImgSamPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
signals:

public slots:
};

#endif // QCFGIMGSAMPAGE_H
