#include "ImageProcessingWidget.h"
#include "ReviewTool.h"


#pragma execution_character_set("utf-8")


#define _CArmSharpen1String       "1"       //!< 锐化1字符
#define _CArmSharpen2String       "2"       //!< 锐化2字符
#define _CArmSharpen3String       "3"       //!< 锐化3字符
#define _CArmDenoise1String       "1"       //!< 降噪1字符
#define _CArmDenoise2String       "2"       //!< 降噪2字符
#define _CArmDenoise3String       "3"       //!< 降噪3字符


ImageProcessingWidget::ImageProcessingWidget(CArmBasePage* page, QAbstractButton* btn, QWidget *parent)
    : QFrame(parent)
    , m_pPageWidget(page)
    , m_pCtrlBtn(btn)
    , m_pShpMenu(new QFrame(parent))
    , m_pDnsMenu(new QFrame(parent))
{
    initUI();
    initBtnGroup();
    initLogic();
}

ImageProcessingWidget::~ImageProcessingWidget()
{
    delete m_pShpMenu;
    delete m_pDnsMenu;
}

void ImageProcessingWidget::initUI()
{
    m_ui.setupUi(this);

    m_dnsUI.setupUi(m_pDnsMenu);
    m_shpUI.setupUi(m_pShpMenu);

    m_pDnsMenu->hide();
    m_pShpMenu->hide();

    this->installEventFilter(this);
    m_ui.shpSlider->installEventFilter(this);
    m_ui.dnsSlider->installEventFilter(this);
    m_ui.contsSlider->installEventFilter(this);
    m_ui.brtSlider->installEventFilter(this);
    m_ui.shpBtn->installEventFilter(this);
    m_ui.dnsBtn->installEventFilter(this);
    m_ui.mt0Btn->installEventFilter(this);
    m_ui.mt1Btn->installEventFilter(this);
    m_ui.mt2Btn->installEventFilter(this);
    m_ui.mt3Btn->installEventFilter(this);

}

void ImageProcessingWidget::initLogic()
{
    connect(m_ui.contsSlider, SIGNAL(valueChanged(int)), this, SLOT(contsValueChanged(int)));
    connect(m_ui.brtSlider, SIGNAL(valueChanged(int)), this, SLOT(brtValueChanged(int)));
    connect(m_ui.shpSlider, SIGNAL(valueChanged(int)), this, SLOT(shpValueChanged(int)));
    connect(m_ui.dnsSlider, SIGNAL(valueChanged(int)), this, SLOT(densValueChanged(int)));

    connect(&m_shpBtnGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(sharpenMenuClicked(QAbstractButton*)));
    connect(&m_dnsBtnGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(denoiseMenuClicked(QAbstractButton*)));
    connect(&m_calibBtnGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(metalCalibClicked(QAbstractButton*)));

    connect(m_ui.shpBtn, SIGNAL(clicked()), this, SLOT(sharpenClicked()));
    connect(m_ui.dnsBtn, SIGNAL(clicked()), this, SLOT(denoiseClicked()));

}

void ImageProcessingWidget::initBtnGroup()
{
    m_shpBtnGroup.addButton(m_shpUI.shp1Btn);
    m_shpBtnGroup.addButton(m_shpUI.shp2Btn);
    m_shpBtnGroup.addButton(m_shpUI.shp3Btn);
    m_shpBtnGroup.setExclusive(true);

    m_dnsBtnGroup.addButton(m_dnsUI.dns1Btn);
    m_dnsBtnGroup.addButton(m_dnsUI.dns2Btn);
    m_dnsBtnGroup.addButton(m_dnsUI.dns3Btn);
    m_dnsBtnGroup.setExclusive(true);

    m_calibBtnGroup.addButton(m_ui.mt0Btn);
    m_calibBtnGroup.addButton(m_ui.mt1Btn);
    m_calibBtnGroup.addButton(m_ui.mt2Btn);
    m_calibBtnGroup.addButton(m_ui.mt3Btn);
    m_calibBtnGroup.setExclusive(true);
}

bool ImageProcessingWidget::eventFilter(QObject * obj, QEvent * event)
{
    switch (event->type())
    {
    case QEvent::FocusOut:
        if (m_pCtrlBtn != nullptr) m_pCtrlBtn->setChecked(false);
        this->hide();
        break;
    case QEvent::Hide:
        m_pDnsMenu->hide();
        m_pShpMenu->hide();
        break;
    case QEvent::MouseButtonPress:
        if (m_pShpMenu->isVisible()) m_pShpMenu->hide();
        if (m_pDnsMenu->isVisible()) m_pDnsMenu->hide();
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

void ImageProcessingWidget::adjustSharpen()
{
    int value = m_ui.shpSlider->value();
    QString sMode = m_ui.shpBtn->text();
    CArmSharpenEnum eMode;
    if (sMode == _CArmSharpen1String)         eMode = CArmSharpenEnum::IT_Sharpen_0;
    else if(sMode == _CArmSharpen2String)     eMode = CArmSharpenEnum::IT_Sharpen_1;
    else if(sMode == _CArmSharpen3String)     eMode = CArmSharpenEnum::IT_Sharpen_2;

    if (m_pPageWidget != nullptr) m_pPageWidget->enableTool(SharpenTool::create(m_pPageWidget,value, eMode));
}

void ImageProcessingWidget::adjustContrast()
{
    int value = m_ui.contsSlider->value();
    if (m_pPageWidget != nullptr) m_pPageWidget->enableTool(ContrastTool::create(m_pPageWidget,value));
}

void ImageProcessingWidget::adjustBright()
{
    int value = m_ui.brtSlider->value();
    if (m_pPageWidget != nullptr) m_pPageWidget->enableTool(BrightTool::create(m_pPageWidget, value));
}

void ImageProcessingWidget::adjustDenoise()
{
    int value = m_ui.dnsSlider->value();
    QString sMode = m_ui.dnsBtn->text();
    CArmDenoiseEnum eMode;
    if (sMode == _CArmDenoise1String)          eMode = CArmDenoiseEnum::IT_Denoise_0;
    else if (sMode == _CArmDenoise2String)     eMode = CArmDenoiseEnum::IT_Denoise_1;
    else if (sMode == _CArmDenoise3String)     eMode = CArmDenoiseEnum::IT_Denoise_2;

    if (m_pPageWidget != nullptr) m_pPageWidget->enableTool(DenoiseTool::create(m_pPageWidget, value, eMode));
}

void ImageProcessingWidget::adjustMetalCalib()
{
}

void ImageProcessingWidget::contsValueChanged(int value)
{
    QString sign = value < 0 ? "" : "+";
    m_ui.contsValue->setText(sign + QString::number(value));

    adjustContrast();
}

void ImageProcessingWidget::brtValueChanged(int value)
{
    QString sign = value < 0 ? "" : "+";
    m_ui.brtValue->setText(sign + QString::number(value));
    
    adjustBright();
}

void ImageProcessingWidget::shpValueChanged(int value)
{
    m_ui.shpValue->setText(QString::number(value));

    adjustSharpen();
}

void ImageProcessingWidget::densValueChanged(int value)
{
    m_ui.dnsValue->setText(QString::number(value));

    adjustDenoise();
}

void ImageProcessingWidget::sharpenMenuClicked(QAbstractButton * btn)
{
    if (btn == m_shpUI.shp1Btn)
    {
        m_ui.shpBtn->setText(_CArmSharpen1String);
    }
    else if (btn == m_shpUI.shp2Btn)
    {
        m_ui.shpBtn->setText(_CArmSharpen2String);
    }
    else if (btn == m_shpUI.shp3Btn)
    {
        m_ui.shpBtn->setText(_CArmSharpen3String);
    }
    
    adjustSharpen();
}

void ImageProcessingWidget::denoiseMenuClicked(QAbstractButton * btn)
{
    if (btn == m_dnsUI.dns1Btn)
    {
        m_ui.dnsBtn->setText(_CArmDenoise1String);
    }
    else if (btn == m_dnsUI.dns2Btn)
    {
        m_ui.dnsBtn->setText(_CArmDenoise2String);
    }
    else if (btn == m_dnsUI.dns3Btn)
    {
        m_ui.dnsBtn->setText(_CArmDenoise3String);
    }

    adjustDenoise();
}

void ImageProcessingWidget::metalCalibClicked(QAbstractButton * btn)
{
    if (m_pPageWidget == nullptr) return;

    if (btn == m_ui.mt0Btn)             m_pPageWidget->enableTool(MetalCalibTool::create(m_pPageWidget, 1, CArmMetalCalibEnum::IT_MetalCalib_0));
    else if (btn == m_ui.mt1Btn)        m_pPageWidget->enableTool(MetalCalibTool::create(m_pPageWidget, 1, CArmMetalCalibEnum::IT_MetalCalib_1));
    else if (btn == m_ui.mt2Btn)        m_pPageWidget->enableTool(MetalCalibTool::create(m_pPageWidget, 1, CArmMetalCalibEnum::IT_MetalCalib_2));
    else if (btn == m_ui.mt3Btn)        m_pPageWidget->enableTool(MetalCalibTool::create(m_pPageWidget, 1, CArmMetalCalibEnum::IT_MetalCalib_3));
}

void ImageProcessingWidget::sharpenClicked()
{
    if (m_pShpMenu->isVisible())
    {
        return;
    }

    QPoint pos = this->mapToParent(m_ui.shpBtn->pos() + QPoint(m_ui.shpBtn->width(), m_ui.shpBtn->height()));
    pos = pos - QPoint(m_pShpMenu->width(), 0);
    m_pShpMenu->move(pos);
    m_pShpMenu->raise();
    m_pShpMenu->show();
}

void ImageProcessingWidget::denoiseClicked()
{
    if (m_pDnsMenu->isVisible())
    {
        return;
    }

    QPoint pos = this->mapToParent(m_ui.dnsBtn->pos() + QPoint(m_ui.dnsBtn->width(), m_ui.dnsBtn->height()));
    pos = pos - QPoint(m_pDnsMenu->width(), 0);
    m_pDnsMenu->move(pos);
    m_pDnsMenu->raise();
    m_pDnsMenu->show();
}
