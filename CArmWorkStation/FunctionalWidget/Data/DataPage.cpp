#include "DataPage.h"

DataPage::DataPage(CArmWorkStationProc* proc):
    WorkStationProc(proc)
    , page(new DataPageWidget(proc))
    , tool(new DataToolWidget())
{
    initLogic();
}

DataPage::~DataPage()
{

}

void DataPage::initLogic()
{
    connect(this, SIGNAL(sigStudyImage(DB_PatientComplete)), page, SIGNAL(sigStudyImage(DB_PatientComplete)));  //查看检查
    connect(this, SIGNAL(sigClickDataBtn()), page, SIGNAL(sigClickDataBtn()));  //点击“图像目录”按钮
}

QWidget * DataPage::getPageWidget()
{
    return page;
}

QWidget * DataPage::getToolWidget()
{
    return tool;
}
