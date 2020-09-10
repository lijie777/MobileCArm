#include "RotateDialog.h"
#include "ReviewTool.h"




/***************************************************************************/
/****************************图像旋转，角度调节窗口************************/
/*************************************************************************/
RotateDialog::RotateDialog(CArmBasePage *page, QAbstractButton* ctlBtn, QWidget *parent)
    : QWidget(parent)
    , m_pPage(page)
    , m_pCtlBtn(ctlBtn)
{
    initUI();
    initLogic();
}

RotateDialog::~RotateDialog()
{
}

void RotateDialog::pressCloseBtn()
{
    this->hide();
    m_pCtlBtn->setChecked(false);
}

void RotateDialog::sliderValueChanged(int value)
{
    QString sign = "";
    if (value > 0) sign = "+";
    ui.value->setText(sign + QString::number((value)) + "°");
    m_pCtlBtn->setText(sign + QString::number((value)));
    m_pPage->enableTool(RotateTool::create(m_pPage, value));

    update();
}

void RotateDialog::initUI()
{
    ui.setupUi(this);
}

void RotateDialog::initLogic()
{
    connect(ui.clsBtn, SIGNAL(clicked()), this, SLOT(pressCloseBtn()));
    connect(ui.slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
}
