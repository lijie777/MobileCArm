#pragma once

#include <QObject>

class DataSource;

class ControlSource : public QObject
{
    Q_OBJECT

public:
    ControlSource();
    virtual ~ControlSource() = 0;

signals:
    void dataSourceChanged(DataSource* src);
    void acquireData();
};