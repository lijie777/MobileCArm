#include "CArmPulseChooseDialog.h"

CArmPulseChooseDialog::CArmPulseChooseDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    init();

}

CArmPulseChooseDialog::~CArmPulseChooseDialog()
{
}

// 脉冲频率选择
void CArmPulseChooseDialog::showPPSDialog()
{
    m_pDoseDialog->show();

    QRect rect = this->geometry();
    QPoint pos = this->mapToGlobal(rect.topLeft());
    m_pDoseDialog->move(pos.x() - 41, pos.y() + 59);
}

void CArmPulseChooseDialog::doseChoose(void)
{
    QPushButton *pushButton = static_cast<QPushButton *>(sender());
    if (pushButton == m_pDoseDialog->ui.pps2Btn)
    {
        ui.PPSBtn->setText(tr("2PPS"));
        emit ppsValueSignal(2);
    }
    if (pushButton == m_pDoseDialog->ui.pps4Btn)
    {
        ui.PPSBtn->setText(tr("4PPS"));
        emit ppsValueSignal(4);
    }
    if (pushButton == m_pDoseDialog->ui.pps8Btn)
    {
        ui.PPSBtn->setText(tr("8PPS"));
        emit ppsValueSignal(8);
    }
    if (pushButton == m_pDoseDialog->ui.pps12Btn)
    {
        ui.PPSBtn->setText(tr("12PPS"));
        emit ppsValueSignal(12);
    }

    m_pDoseDialog->hide();
}

void CArmPulseChooseDialog::init(void)
{
    m_pDoseDialog = new CArmDose(this);

    // 脉冲频率选择
    connect(ui.PPSBtn, &QPushButton::clicked, this, &CArmPulseChooseDialog::showPPSDialog);
    connect(ui.PPS_Btn, &QPushButton::clicked, this, &CArmPulseChooseDialog::showPPSDialog);

    connect(m_pDoseDialog->ui.pps2Btn, &QPushButton::clicked, this, &CArmPulseChooseDialog::doseChoose);
    connect(m_pDoseDialog->ui.pps4Btn, &QPushButton::clicked, this, &CArmPulseChooseDialog::doseChoose);
    connect(m_pDoseDialog->ui.pps8Btn, &QPushButton::clicked, this, &CArmPulseChooseDialog::doseChoose);
    connect(m_pDoseDialog->ui.pps12Btn, &QPushButton::clicked, this, &CArmPulseChooseDialog::doseChoose);
}
