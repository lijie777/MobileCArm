#include "ReviewToolWidget.h"
#include "ReviewTool.h"
#include "QDate"

/*************************************************************
* class   ButtonGroup   阅片工具按钮管理
**************************************************************/
void ButtonGroup::addAlternationButton(QAbstractButton * button)
{
    alt_button_list.push_back(button);
}

void ButtonGroup::addNeitherButton(QAbstractButton * button)
{
    net_button_list.push_back(button);

}

void ButtonGroup::removeButtom(QAbstractButton * button)
{


    alt_button_list.remove(button);
    net_button_list.remove(button);

}

bool ButtonGroup::clickedButton(QAbstractButton * button)
{
    bool button_status = false;

    for (auto it = alt_button_list.begin();it != alt_button_list.end();it++)
    {
        if ((*it) != button)
        {
            (*it)->setChecked(false);
            continue;
        }
        button_status = (*it)->isChecked();
    }

    for (auto it = net_button_list.begin();it != net_button_list.end();it++)
    {
        (*it)->setChecked(false);
    }

    return button_status;
}

void ButtonGroup::closeButtons()
{
    for (auto it = alt_button_list.begin();it != alt_button_list.end();it++)
    {
        (*it)->setChecked(false);
    }
    for (auto it = net_button_list.begin();it != net_button_list.end();it++)
    {
        (*it)->setChecked(false);
    }
}

void ButtonGroup::setButtonEnable(bool en)
{
    for (auto & i : alt_button_list)
    {
        i->setEnabled(en);
    }
    for (auto & i : net_button_list)
    {
        i->setEnabled(en);
    }
}



ReviewToolWidget::ReviewToolWidget(CArmBasePage* page, QWidget *parent)
    : QFrame(parent)
    , m_pPageWidget(page)
    , m_imgPrcWidget(nullptr)
    , m_rotateWidget(nullptr)
{
    initUI();
    initLogic();
    initBtnGroup();


    m_imgPrcWidget = new ImageProcessingWidget(page, ui.PrcBtn, parent);
    m_imgPrcWidget->hide();

    m_rotateWidget = new RotateDialog(page, ui.RtBtn, page);
    m_rotateWidget->hide();

}

ReviewToolWidget::~ReviewToolWidget()
{
    delete m_imgPrcWidget;
    delete m_rotateWidget;
}

void ReviewToolWidget::updatePatientInfo(const CP_PatientData & info)
{
    if (info._patientID.empty())
    {
        ui.name->setText(tr("无患者"));
        ui.emg->hide();
        ui.info->hide();
        this->setEnabled(false);
        return;
    }

    ui.name->setText(QString::fromLocal8Bit(info._patientName.c_str()));

    if (info._patientSex == "F") ui.gender->setText(tr("女"));
    else if (info._patientSex == "M") ui.gender->setText(tr("男"));
    else ui.gender->setText(tr("其他"));
    ui.age->setText(QString::fromLocal8Bit(info._patientAge.c_str()));
    ui.birth->setText(QDate::fromString(info._ptBirthDate.c_str(), "yyyyMMdd").toString("yyyy-MM-dd"));
    ui.emg->setVisible(info._isEmergency);
    this->setEnabled(true);
}

void ReviewToolWidget::clearPatientInfo()
{
}

void ReviewToolWidget::initUI()
{
    ui.setupUi(this);

    ui.info->hide();
    ui.emg->hide();

    this->setEnabled(true);
}

void ReviewToolWidget::initLogic()
{
    //历史列表
    connect(ui.hisBtn, SIGNAL(clicked()), this, SLOT(pressDragBtn()));

    //标记
    connect(ui.LBtn, SIGNAL(clicked()), this, SLOT(pressLBtn()));
    connect(ui.ABtn, SIGNAL(clicked()), this, SLOT(pressABtn()));
    connect(ui.HBtn, SIGNAL(clicked()), this, SLOT(pressHBtn()));
    connect(ui.RBtn, SIGNAL(clicked()), this, SLOT(pressRBtn()));
    connect(ui.PBtn, SIGNAL(clicked()), this, SLOT(pressPBtn()));
    connect(ui.FBtn, SIGNAL(clicked()), this, SLOT(pressFBtn()));
    connect(ui.ArrBtn, SIGNAL(clicked()), this, SLOT(pressArrowBtn()));
    connect(ui.TxBtn, SIGNAL(clicked()), this, SLOT(pressTextBtn()));
    connect(ui.HdBtn, SIGNAL(clicked()), this, SLOT(pressHideBtn()));

    //测量
    connect(ui.MsBtn, SIGNAL(clicked()), this, SLOT(pressLineBtn()));
    connect(ui.AgBtn, SIGNAL(clicked()), this, SLOT(pressAngleBtn()));
    connect(ui.CkBtn, SIGNAL(clicked()), this, SLOT(pressCalibBtn()));
    connect(ui.DtBtn, SIGNAL(clicked()), this, SLOT(pressCalibBtn()));

    //图像处理
    connect(ui.PrcBtn, SIGNAL(clicked()), this, SLOT(pressPrcBtn()));
    
    connect(ui.RtBtn, SIGNAL(clicked()), this, SLOT(pressRtBtn()));
    connect(ui.RvBtn, SIGNAL(clicked()), this, SLOT(pressRvBtn()));
    connect(ui.FHBtn, SIGNAL(clicked()), this, SLOT(pressFHBtn()));
    connect(ui.FVBtn, SIGNAL(clicked()), this, SLOT(pressFVBtn()));
    connect(ui.RcBtn, SIGNAL(clicked()), this, SLOT(pressRcBtn()));
}

void ReviewToolWidget::initBtnGroup()
{
    // 标记
    m_buttonGroup.addAlternationButton(ui.LBtn);
    m_buttonGroup.addAlternationButton(ui.ABtn);
    m_buttonGroup.addAlternationButton(ui.HBtn);
    m_buttonGroup.addAlternationButton(ui.RBtn);
    m_buttonGroup.addAlternationButton(ui.PBtn);
    m_buttonGroup.addAlternationButton(ui.FBtn);
    m_buttonGroup.addAlternationButton(ui.ArrBtn);
    m_buttonGroup.addAlternationButton(ui.TxBtn);

    // 测量
    m_buttonGroup.addAlternationButton(ui.MsBtn);
    m_buttonGroup.addAlternationButton(ui.CkBtn);
    m_buttonGroup.addAlternationButton(ui.AgBtn);

    // 查看
    m_buttonGroup.addAlternationButton(ui.MvBtn);
    m_buttonGroup.addAlternationButton(ui.MgBtn);
    m_buttonGroup.addNeitherButton(ui.HdBtn);
    m_buttonGroup.addNeitherButton(ui.DtBtn);


    //// 图像处理
    //m_buttonGroup.addAlternationButton(ui.RvBtn);
    //m_buttonGroup.addAlternationButton(ui.FHBtn);
    //m_buttonGroup.addAlternationButton(ui.FVBtn);
    m_buttonGroup.addAlternationButton(ui.RtBtn);
}

void ReviewToolWidget::pressLBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.LBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(DirTxMarkTool::create(DirTxMarkTool::DirType::DirL, ui.LBtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressABtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.ABtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(DirTxMarkTool::create(DirTxMarkTool::DirType::DirA, ui.ABtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressHBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.HBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(DirTxMarkTool::create(DirTxMarkTool::DirType::DirH, ui.HBtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressRBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.RBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(DirTxMarkTool::create(DirTxMarkTool::DirType::DirR, ui.RBtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressPBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.PBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(DirTxMarkTool::create(DirTxMarkTool::DirType::DirP, ui.PBtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressFBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.FBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(DirTxMarkTool::create(DirTxMarkTool::DirType::DirF, ui.FBtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressArrowBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.ArrBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(ArrowMarkTool::create(ui.ArrBtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressTextBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.TxBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(CommentTextTool::create(ui.TxBtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressHideBtn()
{
    m_buttonGroup.clickedButton(ui.HdBtn);
    m_pPageWidget->enableTool(HideTool::create());
}

void ReviewToolWidget::pressLineBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.MsBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(LineMeasureTool::create(ui.MsBtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressAngleBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.AgBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(AngleMeasureTool::create(ui.AgBtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressCalibBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.CkBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(ImgCalibTool::create(m_pPageWidget, ui.CkBtn));
    }
    else
    {
        m_pPageWidget->enableTool(CloseTool::create());
    }
}

void ReviewToolWidget::pressDeleteBtn()
{
    bool tool_status = m_buttonGroup.clickedButton(ui.DtBtn);
    if (tool_status)
    {
        m_pPageWidget->enableTool(DeleteTool::create());
    }
}

void ReviewToolWidget::pressHisBtn()
{

}

void ReviewToolWidget::pressPrcBtn()
{
    if (ui.PrcBtn->isChecked())
    {
        QPoint glPos = this->mapToGlobal(QPoint(0, 0));
        int y = glPos.y() + this->height() - m_imgPrcWidget->height();
        int x = glPos.x() - m_imgPrcWidget->width();
        QPoint movePos = m_imgPrcWidget->parentWidget()->mapFromGlobal(QPoint(x, y));
        m_imgPrcWidget->move(movePos);
        m_imgPrcWidget->raise();
        m_imgPrcWidget->setFocus();
        m_imgPrcWidget->show();
    }
    else
    {
        m_imgPrcWidget->hide();
    }
}

void ReviewToolWidget::pressRtBtn()
{
    bool status = m_buttonGroup.clickedButton(ui.RtBtn);
    if (status)
    {
        m_rotateWidget->move(m_pPageWidget->pos());
        m_rotateWidget->resize(m_pPageWidget->size());
        m_rotateWidget->show();
    }
    else
    {
        m_rotateWidget->hide();
    }
}

void ReviewToolWidget::pressRvBtn()
{
    bool status = ui.RvBtn->isChecked();
    m_pPageWidget->enableTool(NegativeTool::create(m_pPageWidget, status));
}

void ReviewToolWidget::pressFHBtn()
{
    bool en = ui.FHBtn->isChecked();
    m_pPageWidget->enableTool(FlipTool::create(m_pPageWidget, FlipTool::MIRROR_TYPE::MT_HORIZONTAL, en));

}

void ReviewToolWidget::pressFVBtn()
{
    bool en = ui.FVBtn->isChecked();
    m_pPageWidget->enableTool(FlipTool::create(m_pPageWidget, FlipTool::MIRROR_TYPE::MT_VERTICAL, en));

}

void ReviewToolWidget::pressRcBtn()
{
    m_pPageWidget->enableTool(RecoverTool::create());
}

void ReviewToolWidget::pressSvBtn()
{
    m_pPageWidget->saveToFile();
}
