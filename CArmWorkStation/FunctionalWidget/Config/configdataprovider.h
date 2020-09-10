#ifndef CONFIGDATAPROVIDER_H
#define CONFIGDATAPROVIDER_H

//#define Tr() 
#include <QObject>

namespace SysConfig {


class ConfigDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit ConfigDataProvider(QObject *parent = 0);
    void setConfigMenuList(const QList<QString>);
    const QList<QString> getConfigMenuList();
    void setScanOrganList(const QList<QString>);
    const QList<QString> getScanOrganList();
    void setScanModeList(const QList<QString>);
    const QList<QString> getScanModeList();

signals:

public slots:

private:
	QList<QString> configMenuList;
    QList<QString> scanOrganList;
    QList<QString> scanModeList;
};

} // namespace SysConfig

#endif // CONFIGDATAPROVIDER_H
