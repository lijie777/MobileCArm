#include "CArmPartChooseDialog.h"
#include <QGridLayout>

#pragma execution_character_set("utf-8")

CArmPartChooseDialog::CArmPartChooseDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    initBodyPartDialog();

}

CArmPartChooseDialog::~CArmPartChooseDialog()
{
    if (m_pPartWidget != nullptr)
    {
        delete []m_pPartWidget;
        m_pPartWidget = nullptr;
    }
}

void CArmPartChooseDialog::initBodyPartDialog(void)
{
    // 隐藏标题栏
    setWindowFlags(Qt::FramelessWindowHint);
    // 置顶当前窗口
    setWindowFlags(Qt::ToolTip);

  //  setModal(true);

    m_pPartWidget = new CArmPartWidget[9];
    QGridLayout *layout = new QGridLayout;

    m_pPartWidget[0].setPicture(":/modelParts/Resource/modelParts/limbs.png");
    m_pPartWidget[0].setDescription(tr("四肢"));
    layout->addWidget(&m_pPartWidget[0], 0, 0);
    m_pPartWidget[1].setPicture(":/modelParts/Resource/modelParts/neck.png");
    m_pPartWidget[1].setDescription(tr("颈部"));
    layout->addWidget(&m_pPartWidget[1], 0, 1);
    m_pPartWidget[2].setPicture(":/modelParts/Resource/modelParts/head.png");
    m_pPartWidget[2].setDescription(tr("头部"));
    layout->addWidget(&m_pPartWidget[2], 0, 2);
    m_pPartWidget[3].setPicture(":/modelParts/Resource/modelParts/trunk.png");
    m_pPartWidget[3].setDescription(tr("躯干"));
    layout->addWidget(&m_pPartWidget[3], 1, 0);
    m_pPartWidget[4].setPicture(":/modelParts/Resource/modelParts/heart.png");
    m_pPartWidget[4].setDescription(tr("心脏"));
    layout->addWidget(&m_pPartWidget[4], 1, 1);
    m_pPartWidget[5].setPicture(":/modelParts/Resource/modelParts/abdomen.png");
    m_pPartWidget[5].setDescription(tr("腹部"));
    layout->addWidget(&m_pPartWidget[5], 1, 2);
    m_pPartWidget[6].setPicture(":/modelParts/Resource/modelParts/urinary.png");
    m_pPartWidget[6].setDescription(tr("泌尿系统"));
    layout->addWidget(&m_pPartWidget[6], 2, 0);
    m_pPartWidget[7].setPicture(":/modelParts/Resource/modelParts/soft_tissue.png");
    m_pPartWidget[7].setDescription(tr("软组织"));
    layout->addWidget(&m_pPartWidget[7], 2, 1);
    m_pPartWidget[8].setPicture(":/modelParts/Resource/modelParts/endoscope.png");
    m_pPartWidget[8].setDescription(tr("内窥镜"));
    layout->addWidget(&m_pPartWidget[8], 2, 2);


    for (auto i = 0; i < 9; ++i)
    {
        m_pPartWidget[i].setFixedSize(96, 96);
        m_pPartWidget[i].styleSet();
        // 信号转发
        connect(&m_pPartWidget[i], &CArmPartWidget::bodyPartSignal, this, &CArmPartChooseDialog::bodyPartSlot);
    }
    layout->setMargin(0);
    ui.frame->setLayout(layout);   
    
    ui.frame->setStyleSheet("QFrame{border:1px solid rgba(151,151,151,1); border-radius:3px}");
}

void CArmPartChooseDialog::bodyPartSlot(void)
{
    CArmPartWidget* partWidget = dynamic_cast<CArmPartWidget*>(sender());

    // 发送当前选择的一个部位的信号
    emit bodyPartSignal(partWidget);
}
