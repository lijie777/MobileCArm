#pragma once
#include <QThread>
#include <QTableWidget>
#include "../Common/Include/CArmCommon.h"
#include "../Common/Include/Config.h"
#include "ViewToolCreator.h"
class CArmLoadSerialData : public QThread
{
public:
    CArmLoadSerialData(QTableWidget *serialTabel, std::vector<CP_ReviewData>);

private:
    void run() override;

private:
    QTableWidget  *m_pNeedLoadDataTab;

    std::vector<CP_ReviewData> m_needLoadDataVec;
};