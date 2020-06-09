#ifndef QCONFIGPAGEABSTRACT_H
#define QCONFIGPAGEABSTRACT_H

#include <QWidget>
#include "Config.h"
#define ToQSTR(v) (QString::fromStdString(v))
#define QStr2Std(v) (v.QString::toStdString())

#define UNOVERWRITEVIRTUAL()     virtual void updatePage(){};  \
                                virtual void savePage(){};    \
                                virtual void enteryPage(){};  \
                                virtual void exitPage(){};

namespace Ui {
    class SystemConfigWidget;
} // namespace Ui

class QConfigPageAbstract : public QWidget
{
    Q_OBJECT
public:
    explicit QConfigPageAbstract(QWidget *parent = 0);
    virtual ~QConfigPageAbstract();

    virtual void updatePage() = 0;
    virtual void savePage() = 0;

    virtual void enteryPage() = 0;
    virtual void exitPage() = 0;

    virtual void init();

    static void setUiPtr(Ui::SystemConfigWidget *);

    static void saveSystemCfg();
    static void updateSysCfg();

signals:
    void entery(QConfigPageAbstract *);
    void exit(QConfigPageAbstract *);

public slots:

public:
    static CArmConfig cfg;
    static Ui::SystemConfigWidget *ui;
};

#endif // QCONFIGPAGEABSTRACT_H
