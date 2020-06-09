#include "ReviewPageWidget.h"
#include "Common\CArmPrinter.h"
#pragma execution_character_set("utf-8")

ReviewPageWidget::ReviewPageWidget(CArmWorkStationProc* proc, QWidget *parent)
    : CArmBasePage(parent)
    , m_currReview(nullptr)
    , m_pImageViewer(new FunctionalView(proc, 1, this))
    , m_pTextureImportor(ViewToolApi::createImage2DTextureImporter(m_pImageViewer))
{
    ui.setupUi(this);
    m_pImageViewer->raise();
    m_pImageViewer->resize(this->width(),this->height());
    ui.stackedWidget->setCurrentWidget(ui.idlePage);
}

ReviewPageWidget::~ReviewPageWidget()
{
    delete m_pImageViewer;
}

void ReviewPageWidget::enableTool(IReadToolPtr tool)
{
    tool.setViewer(m_pImageViewer);
    tool.startWork();
}

void ReviewPageWidget::saveToFile()
{
    for (auto & i : m_reviewData)
    {
        CArmPrintData out(i);
        CArmReviewPrinter::Serialize(out, "");
    }
}

void ReviewPageWidget::updatePatientInfo(const CP_PatientData & info)
{
    ui.name->setText(QString::fromLocal8Bit(info._patientName.c_str()));
    ui.stackedWidget->setCurrentWidget(ui.exposePage);
}

void ReviewPageWidget::clearPatientInfo()
{
    // 清空界面
    ui.name->setText("");
    ui.stackedWidget->setCurrentWidget(ui.idlePage);

    // 清空图像数据
    clearReviewInfo();
}

void ReviewPageWidget::clearReviewInfo()
{
    // 清空显示
    m_pImageViewer->deleteCTTexture();

    // 清空图像数据
    for (auto & i : m_reviewData)
    {

    }
}

void ReviewPageWidget::loadReviewData(CP_ReviewData* info)
{
    m_currReview = info;

    // 查询数据服务
}

void ReviewPageWidget::loadNewScanData(const std::list<NewScanData>& lstTx)
{
    // 清空当前数据
    for (auto & i : m_reviewData)
    {
        if (i->_imageData._imageData != nullptr) delete i->_imageData._imageData;
    }
    m_reviewData.clear();
    
    for (auto & i : lstTx)
    {
        // 加载新图像
        CP_ReviewData* data = CArmCommon::GetReviewData(i);

        // 创建图像纹理
        TextureObject3DPtr texture = CArmCommon::CreateTextureObject(data->_imageData);
        data->_texture = texture;
        if (m_pTextureImportor.createTexture(texture) == false)
        {
            //报错
        }
    }

    // 显示图像到窗口
    //m_pImageViewer->loadReviewData(data);


    //ui.stackedWidget->setCurrentWidget(ui.reivewPage);
    //m_pImageViewer->loadReviewData(info);
}


