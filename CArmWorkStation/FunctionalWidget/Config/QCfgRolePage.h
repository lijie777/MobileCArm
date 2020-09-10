#ifndef QCFGROLEPAGE_H
#define QCFGROLEPAGE_H

#include "QConfigPageAbstract.h"
#include <vector>
#include <string>
#include <list>
#include <QString>
#include <QVector>
#include <QMap>
class QCfgRolePage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgRolePage(QWidget *parent = 0);
    ~QCfgRolePage();

    void updatePage()override;
    void savePage()override;

    void enteryPage()override;
    void exitPage()override;

    void init()override;
private:
    void initTitle();

    QVector<QString> titleList;                        //!< 一级菜单
    QMap<QString, QVector<QString>> titleInfoMap;      //!< 二级菜单
    QMap<QString, QVector<QString>> titleMeauMap;      //!< 三级菜单


signals:

public slots:
};

#endif // QCFGROLEPAGE_H
