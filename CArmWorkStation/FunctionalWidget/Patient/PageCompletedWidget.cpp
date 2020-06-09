#include "PageCompletedWidget.h"
#include "Common\CArmWorkStationCommon.h"
#include "CommonUtility.h"
#include <QListView>
#include <QtCore/QSettings.h>
#include "..\Message\MessageDialog.h"
#include "..\Message\MessageTipDialog.h"

PageCompletedWidget::PageCompletedWidget(CArmWorkStationProc* proc, QWidget *parent)
    : QFrame(parent)
    , WorkStationProc(proc)
    , QueryResultRemind_ui(new Ui::RemindFrame)
{
    initUI();
    initLogic();

    m_nPatientCount = 0;
    m_pPatientsComplete = nullptr;
    currentPatientComplete = nullptr;
}

PageCompletedWidget::~PageCompletedWidget()
{
    //��ղ�ѯ�������
    m_nPatientCount = 0;
    if (m_pPatientsComplete != nullptr)
    {
        delete[] m_pPatientsComplete;
        m_pPatientsComplete = nullptr;
    }
}

void PageCompletedWidget::initUI()
{
    ui.setupUi(this);

    ui.GenderBox->setView(new QListView()); //�����������
    ui.SourceBox->setView(new QListView()); //�����������

    StudyDateBeginAction = new QAction(ui.StudyDateBeginEdit);
    StudyDateEndAction = new QAction(ui.StudyDateEndEdit);
    StudyDateBeginAction->setIcon(QIcon(":/CArmWorkStation/ImageSource/Patient/Doc_list_icon_calender.png"));
    StudyDateEndAction->setIcon(QIcon(":/CArmWorkStation/ImageSource/Patient/Doc_list_icon_calender.png"));
    ui.StudyDateBeginEdit->addAction(StudyDateBeginAction, QLineEdit::TrailingPosition);
    ui.StudyDateEndEdit->addAction(StudyDateEndAction, QLineEdit::TrailingPosition);

    mQueryResultRemind = new QFrame(ui.QueryTableWidget);
    QueryResultRemind_ui->setupUi(mQueryResultRemind);
    ui.QueryTableWidget->horizontalHeader()->setHighlightSections(false);
    ui.QueryTableWidget->setFocusPolicy(Qt::NoFocus);
    ui.QueryTableWidget->setColumnWidth(0,65);   //���
    ui.QueryTableWidget->setColumnWidth(1, 95); //���״̬
    ui.QueryTableWidget->setColumnWidth(2, 105); //����
    ui.QueryTableWidget->setColumnWidth(3, 180); //����ID
    ui.QueryTableWidget->setColumnWidth(4, 105); //��鲿λ
    ui.QueryTableWidget->setColumnWidth(5, 100); //����ҽ��
    ui.QueryTableWidget->setColumnWidth(6, 115); //MPPS״̬

    mQueryResultRemind->setFixedWidth(ui.QueryTableWidget->width());
    QueryResultRemind_ui->RemindLabel->setFixedWidth(ui.QueryTableWidget->width());
    mQueryResultRemind->move(0, 150);
    mQueryResultRemind->hide();
}

void PageCompletedWidget::initLogic()
{
    connect(ui.ClearButton, SIGNAL(clicked()), this, SLOT(con_ClearButton_clicked()));  //�������ա���ť
    connect(ui.QueryButton, SIGNAL(clicked()), this, SLOT(con_QueryButton_clicked()));  //�������ѯ����ť
    connect(ui.QueryTableWidget, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(setDataButtonEnable()));     //�����������Ϣ��Widget��ĳ������
    connect(ui.QueryTableWidget, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(setDataButtonEnable()));     //�����ƶ�
    connect(StudyDateBeginAction, SIGNAL(triggered()), this, SLOT(showScheduledDateBeginCalendar()));     //��ʾ���ڿؼ�
    connect(StudyDateEndAction, SIGNAL(triggered()), this, SLOT(showScheduledDateEndCalendar()));     //��ʾ���ڿؼ�
    connect(ui.DeleteButton, SIGNAL(clicked()), this, SLOT(con_DeleteButton_clicked()));  //�����ɾ������ť
    connect(ui.EditButton, SIGNAL(clicked()), this, SLOT(con_EditButton_clicked()));  //������༭����ť
    connect(ui.NewStudyButton, SIGNAL(clicked()), this, SLOT(con_NewStudyButton_clicked()));  //������¼�顱��ť
    connect(ui.RecoverStudyButton, SIGNAL(clicked()), this, SLOT(con_RecoverStudyButton_clicked()));  //������ָ���顱��ť
    connect(ui.StudyButton, SIGNAL(clicked()), this, SLOT(con_StudyButton_clicked()));  //������鿴��顱��ť
}

void PageCompletedWidget::con_NewStudyButton_clicked()
{
    MessageTipDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(tr("��ȷ��Ҫ�½������"));
    int res = msgFrame.exec();

    if (res != QDialog::Accepted)
    {
        return;
    }

    std::string tmpCompleteID;
    ErrorCode errAddPatientToComplete = WorkStationProc->AddPatientToComplete(currentPatientComplete->mDB_Patient.patient_id, tmpCompleteID);
    if (errAddPatientToComplete != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errAddPatientToComplete);
        remindMessage(tr("�½����ʧ�ܣ�"));
        LOG4CPLUS_ERROR(g_logger, L"Failed to add patient to complete");
        return;
    }
    CArmWorkStationCommon::ToStartScan(currentPatientComplete->mDB_Patient, tmpCompleteID);
    con_QueryButton_clicked();
}

void PageCompletedWidget::con_RecoverStudyButton_clicked()
{
    MessageTipDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(tr("��ȷ��Ҫ�ָ������"));
    int res = msgFrame.exec();

    if (res != QDialog::Accepted)
    {
        return;
    }

    ErrorCode errUpdatePatientCompleteStatus = WorkStationProc->UpdatePatientCompleteStatus(currentPatientComplete->patient_complete_id, StudyStatus::Doing);
    if (errUpdatePatientCompleteStatus != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errAddPatientToComplete);
        remindMessage(tr("�ָ����ʧ�ܣ�"));
        LOG4CPLUS_ERROR(g_logger, L"Failed to update patient complete status");
        return;
    }

    ui.QueryTableWidget->setItem(cRow, 1, new QTableWidgetItem(tr("������")));  //���״̬
    ui.QueryTableWidget->item(cRow, 1)->setTextColor(QColor(0, 145, 255)); //��ɫ������

    //���¿�ʼ��Ƭ
    CArmWorkStationCommon::ToStartScan(currentPatientComplete->mDB_Patient, currentPatientComplete->patient_complete_id);
}

void PageCompletedWidget::con_StudyButton_clicked()
{
    emit sigStudyImage(*currentPatientComplete);
}

void PageCompletedWidget::con_EditButton_clicked()
{
    emit sigEditPatientCompleted(currentPatientComplete->mDB_Patient);
}

void PageCompletedWidget::remindMessage(const QString & text)
{
    MessageDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(text);
    msgFrame.exec();
}

void PageCompletedWidget::con_ClearButton_clicked()
{
    clearCondition();
    clearRemind();
    setDataButtonStatus(false);
}

void PageCompletedWidget::con_DeleteButton_clicked()
{
    MessageTipDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(tr("��ȷ��Ҫɾ�����μ����"));
    int res = msgFrame.exec();

    if (res != QDialog::Accepted)
    {
        return;
    }

    ErrorCode errDeletePatientsSchedule = WorkStationProc->DeletePatientsComplete(currentPatientComplete->patient_complete_id);
    if (errDeletePatientsSchedule != ErrorCode::ERR_SUCCESS)
    {
        remindMessage(tr("ɾ�������Ϣʧ�ܣ�"));
    }
    ui.QueryTableWidget->removeRow(cRow);
}

void PageCompletedWidget::clearCondition()
{
    ui.PatientNameEdit->clear();
    ui.PatientIDEdit->clear();
    ui.GenderBox->setCurrentIndex(0);
    ui.DoctorEdit->clear();
    ui.StudyDateBeginEdit->clear();
    ui.StudyDateEndEdit->clear();
    ui.LIMBSCheckBox->setChecked(false);
    ui.NECKCheckBox->setChecked(false);
    ui.HEADCheckBox->setChecked(false);
    ui.TRUNCKCheckBox->setChecked(false);
    ui.HEARTCheckBox->setChecked(false);
    ui.ABDOMENCheckBox->setChecked(false);
    ui.URINARYCheckBox->setChecked(false);
    ui.SOFT_TISSUECheckBox->setChecked(false);
    ui.ENDOSCOPECheckBox->setChecked(false);
    ui.SourceBox->setCurrentIndex(0);
}

void PageCompletedWidget::clearRemind()
{
    ui.QueryTableWidget->clearContents();
    ui.QueryTableWidget->setRowCount(0);
    mQueryResultRemind->hide();
}

void PageCompletedWidget::getCurrentRowInfo()
{
    cRow = ui.QueryTableWidget->currentRow();
    if (cRow < 0)
        return;
    currentPatientComplete = (DB_PatientComplete *)ui.QueryTableWidget->item(cRow, 0)->data(Qt::UserRole).value<void*>();
    currentStudyStatus = currentPatientComplete->study_status;
}

void PageCompletedWidget::setDataButtonStatus(bool m_status)
{
    getCurrentRowInfo();
    if (m_status)
    {
        //ֻ�����жϵĿ��ԡ��ָ���顱
        if (currentStudyStatus == StudyStatus::Interrupt)
        {
            ui.RecoverStudyButton->setEnabled(true);
        }
        else
        {
            ui.RecoverStudyButton->setEnabled(false);
        }

        //ֻ�����жϡ�����ɵĿ��ԡ�ɾ���������¼�顱
        if (currentStudyStatus == StudyStatus::Interrupt|| currentStudyStatus == StudyStatus::Completed)
        {
            ui.DeleteButton->setEnabled(true);
            ui.NewStudyButton->setEnabled(true);
        }
        else
        {
            ui.DeleteButton->setEnabled(false);
            ui.NewStudyButton->setEnabled(false);
        }
        //ֻ������ɵĿ��ԡ��鿴��顱
        if (currentStudyStatus == StudyStatus::Completed)
        {
            ui.StudyButton->setEnabled(true);
        }
        else
        {
            ui.StudyButton->setEnabled(false);
        }

        ui.SendMPPSButton->setEnabled(m_status);
        ui.EditButton->setEnabled(m_status);
    }
    else
    {
        ui.SendMPPSButton->setEnabled(m_status);
        ui.RecoverStudyButton->setEnabled(m_status);
        ui.DeleteButton->setEnabled(m_status);
        ui.EditButton->setEnabled(m_status);
        ui.NewStudyButton->setEnabled(m_status);
        ui.StudyButton->setEnabled(m_status);
    }
}

void PageCompletedWidget::setTableWidgetStyle()
{
    int sum_row = ui.QueryTableWidget->rowCount();
    int sum_column = ui.QueryTableWidget->columnCount();
    QFontMetrics fontMetrics(ui.QueryTableWidget->font());
    for (int i = 0; i < sum_row; i++)
    {
        for (int j = 0; j < sum_column; j++)
        {
            ui.QueryTableWidget->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter); //������ʾ����
            QString tmpStr = ui.QueryTableWidget->item(i, j)->text();
            if (fontMetrics.width(tmpStr) > ui.QueryTableWidget->columnWidth(j))
            {
                ui.QueryTableWidget->item(i, j)->setToolTip(tmpStr);
            }
            else
            {
                //ui.QueryTableWidget->item(i, j)->setToolTip("");
                ui.QueryTableWidget->item(i, j)->setToolTip(tmpStr);
            }
        }
    }
}

void PageCompletedWidget::setDataButtonDisable()
{
    setDataButtonStatus(false);
}

void PageCompletedWidget::setDataButtonEnable()
{
    setDataButtonStatus(true);
}

void PageCompletedWidget::showScheduledDateBeginCalendar()
{
    MyCalendarWidget* StudyDateBeginCalendar = new MyCalendarWidget(ui.StudyDateBeginEdit, ui.StudyDateBeginEdit, ":/CArmWorkStation/ImageSource/Patient");
    StudyDateBeginCalendar->setMinMaxDate(QDate(1900, 1, 1), QDate::currentDate());
    StudyDateBeginCalendar->move(this->mapToGlobal(ui.StudyDateBeginEdit->pos()).x(), this->mapToGlobal(ui.StudyDateBeginEdit->pos()).y() + ui.StudyDateBeginEdit->geometry().height());
    StudyDateBeginCalendar->show();
}

void PageCompletedWidget::showScheduledDateEndCalendar()
{
    MyCalendarWidget* StudyDateEndCalendar = new MyCalendarWidget(ui.StudyDateEndEdit, ui.StudyDateEndEdit, ":/CArmWorkStation/ImageSource/Patient");
    StudyDateEndCalendar->setMinMaxDate(QDate(1900, 1, 1), QDate::currentDate());
    StudyDateEndCalendar->move(this->mapToGlobal(ui.StudyDateEndEdit->pos()).x(), this->mapToGlobal(ui.StudyDateEndEdit->pos()).y() + ui.StudyDateEndEdit->geometry().height());
    StudyDateEndCalendar->show();
}

void PageCompletedWidget::con_QueryButton_clicked()
{
    clearRemind();

    strcpy(mSearchCondition.patient_name, ui.PatientNameEdit->text().toLocal8Bit());
    strcpy(mSearchCondition.patient_id, ui.PatientIDEdit->text().toLocal8Bit());

    //�Ա��ж�
    if (ui.GenderBox->currentText() == "��")
        strcpy(mSearchCondition.patient_sex, "M");
    else if (ui.GenderBox->currentText() == "Ů")
        strcpy(mSearchCondition.patient_sex, "F");
    else
        strcpy(mSearchCondition.patient_sex, "");

    strcpy(mSearchCondition.doctor_name, ui.DoctorEdit->text().toLocal8Bit());

    std::string strStudyDateBegin = ui.StudyDateBeginEdit->text().toLocal8Bit();
    std::string strStudyDateEnd = ui.StudyDateEndEdit->text().toLocal8Bit();

    //�ж��Ƿ������ʽΪYYYY-MM-DD��YYYY/MM/DD��YYYYMMDD(ͳһת����YYYYMMDD)
    bool isInputError = false;
    if (!strStudyDateBegin.empty())
    {
        if (!UCommonUtility::CheckDate(&strStudyDateBegin))
        {
            isInputError = true;
        }
        else if (strStudyDateBegin > UCommonUtility::GetSystemTime().substr(0, 8))
        {
            strStudyDateBegin = UCommonUtility::GetSystemTime().substr(0, 8);
            std::string showDate = UCommonUtility::GetSystemTime().substr(0, 4) + "-" + UCommonUtility::GetSystemTime().substr(4, 2) + "-" + UCommonUtility::GetSystemTime().substr(6, 2);
            ui.StudyDateBeginEdit->setText(QString::fromStdString(showDate));
        }
        else if (strStudyDateBegin < "18000101")
        {
            strStudyDateBegin = "18000101";
            ui.StudyDateBeginEdit->setText("1800-01-01");
        }
    }

    if (strStudyDateEnd.empty())
    {
        strStudyDateEnd = UCommonUtility::GetSystemTime().substr(0, 8);
        std::string showDate = UCommonUtility::GetSystemTime().substr(0, 4) + "-" + UCommonUtility::GetSystemTime().substr(4, 2) + "-" + UCommonUtility::GetSystemTime().substr(6, 2);
        ui.StudyDateEndEdit->setText(QString::fromStdString(showDate));
    }
    else
    {
        if (!UCommonUtility::CheckDate(&strStudyDateEnd))
        {
            isInputError = true;
        }
        else if (strStudyDateEnd > UCommonUtility::GetSystemTime().substr(0, 8))
        {
            strStudyDateEnd = UCommonUtility::GetSystemTime().substr(0, 8);
            std::string showDate = UCommonUtility::GetSystemTime().substr(0, 4) + "-" + UCommonUtility::GetSystemTime().substr(4, 2) + "-" + UCommonUtility::GetSystemTime().substr(6, 2);
            ui.StudyDateEndEdit->setText(QString::fromStdString(showDate));
        }
        else if (strStudyDateEnd < "18000101")
        {
            strStudyDateEnd = "18000101";
            ui.StudyDateEndEdit->setText("1800-01-01");
        }
    }

    //�������ֱ�Ӳ�ѯΪ��
    if (isInputError)
    {
        mQueryResultRemind->show();
        setDataButtonStatus(false);
        return;
    }

    strcpy(mSearchCondition.study_date_begin, strStudyDateBegin.c_str());
    strcpy(mSearchCondition.series_date_end, strStudyDateEnd.c_str());

    if (ui.AllRadioButton->isChecked())
    {
        mSearchCondition.mStudyStatus = StudyStatus::All;
    }
    else if (ui.DoingRadioButton->isChecked())
    {
        mSearchCondition.mStudyStatus = StudyStatus::Doing;
    }
    else if (ui.InterruptRadioButton->isChecked())
    {
        mSearchCondition.mStudyStatus = StudyStatus::Interrupt;
    }
    else if (ui.CompletedRadioButton->isChecked())
    {
        mSearchCondition.mStudyStatus = StudyStatus::Completed;
    }

    std::string strListScanPart;
    if (ui.LIMBSCheckBox->isChecked())
    {
        if (strListScanPart.empty())
        {
            strListScanPart = std::to_string((int)ScanPart::LIMBS);
        }
        else
        {
            strListScanPart = strListScanPart + "," + std::to_string((int)ScanPart::LIMBS);
        }
    }
    if (ui.NECKCheckBox->isChecked())
    {
        if (strListScanPart.empty())
        {
            strListScanPart = std::to_string((int)ScanPart::NECK);
        }
        else
        {
            strListScanPart = strListScanPart + "," + std::to_string((int)ScanPart::NECK);
        }
    }
    if (ui.HEADCheckBox->isChecked())
    {
        if (strListScanPart.empty())
        {
            strListScanPart = std::to_string((int)ScanPart::HEAD);
        }
        else
        {
            strListScanPart = strListScanPart + "," + std::to_string((int)ScanPart::HEAD);
        }
    }
    if (ui.TRUNCKCheckBox->isChecked())
    {
        if (strListScanPart.empty())
        {
            strListScanPart = std::to_string((int)ScanPart::TRUNCK);
        }
        else
        {
            strListScanPart = strListScanPart + "," + std::to_string((int)ScanPart::TRUNCK);
        }
    }
    if (ui.HEARTCheckBox->isChecked())
    {
        if (strListScanPart.empty())
        {
            strListScanPart = std::to_string((int)ScanPart::HEART);
        }
        else
        {
            strListScanPart = strListScanPart + "," + std::to_string((int)ScanPart::HEART);
        }
    }
    if (ui.ABDOMENCheckBox->isChecked())
    {
        if (strListScanPart.empty())
        {
            strListScanPart = std::to_string((int)ScanPart::ABDOMEN);
        }
        else
        {
            strListScanPart = strListScanPart + "," + std::to_string((int)ScanPart::ABDOMEN);
        }
    }
    if (ui.URINARYCheckBox->isChecked())
    {
        if (strListScanPart.empty())
        {
            strListScanPart = std::to_string((int)ScanPart::URINARY);
        }
        else
        {
            strListScanPart = strListScanPart + "," + std::to_string((int)ScanPart::URINARY);
        }
    }
    if (ui.SOFT_TISSUECheckBox->isChecked())
    {
        if (strListScanPart.empty())
        {
            strListScanPart = std::to_string((int)ScanPart::SOFT_TISSUE);
        }
        else
        {
            strListScanPart = strListScanPart + "," + std::to_string((int)ScanPart::SOFT_TISSUE);
        }
    }
    if (ui.ENDOSCOPECheckBox->isChecked())
    {
        if (strListScanPart.empty())
        {
            strListScanPart = std::to_string((int)ScanPart::ENDOSCOPE);
        }
        else
        {
            strListScanPart = strListScanPart + "," + std::to_string((int)ScanPart::ENDOSCOPE);
        }
    }
   
   

    strcpy(mSearchCondition.list_scan_part, strListScanPart.c_str());
    strcpy(mSearchCondition.source, ui.SourceBox->currentText().toLocal8Bit());

    //�������ݿ���в�ѯ
    m_nPatientCount = 0;
    m_pPatientsComplete = nullptr;

    ErrorCode errGetLocalPatientsComplete = WorkStationProc->GetLocalPatientsComplete(mSearchCondition, m_nPatientCount, &m_pPatientsComplete);
    if (errGetLocalPatientsComplete != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errGetLocalPatientsComplete);
        remindMessage(tr("��ѯ���ݿ�ʧ�ܣ�"));
        return;
    }
    
    if (m_nPatientCount > 0)
    {
        ui.QueryTableWidget->setSortingEnabled(false);
        //��ʾ����
        for (int i = 0; i < m_nPatientCount; i++)
        {
            std::string showPatientName = tostring(m_pPatientsComplete[i].mDB_Patient.patient_last_name) + tostring(m_pPatientsComplete[i].mDB_Patient.patient_first_name);
            ui.QueryTableWidget->insertRow(i);
            ui.QueryTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));  //��� 

            QString showStudyStatus;
            if (m_pPatientsComplete[i].study_status == StudyStatus::Doing)
            {
                showStudyStatus = tr("������");
                ui.QueryTableWidget->setItem(i, 1, new QTableWidgetItem(showStudyStatus));  //���״̬
                ui.QueryTableWidget->item(i, 1)->setTextColor(QColor(0, 145, 255)); //��ɫ������
            }
            else if (m_pPatientsComplete[i].study_status == StudyStatus::Interrupt)
            {
                showStudyStatus = tr("���ж�");
                ui.QueryTableWidget->setItem(i, 1, new QTableWidgetItem(showStudyStatus));  //���״̬
                ui.QueryTableWidget->item(i, 1)->setTextColor(QColor(252, 57, 35)); //��ɫ���ж�
            }
            else if (m_pPatientsComplete[i].study_status == StudyStatus::Completed)
            {
                showStudyStatus = tr("�����");
                ui.QueryTableWidget->setItem(i, 1, new QTableWidgetItem(showStudyStatus));  //���״̬
                ui.QueryTableWidget->item(i, 1)->setTextColor(QColor(180, 196, 217)); //Ĭ��ɫ�����
            }

            ui.QueryTableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromLocal8Bit(showPatientName.c_str())));  //����

            ui.QueryTableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromLocal8Bit(m_pPatientsComplete[i].mDB_Patient.patient_id)));  //����ID

            ScanPart tmpScanPart = (ScanPart)m_pPatientsComplete[i].scan_part;
            QString showScanPart;
            switch (tmpScanPart)
            {
            case ScanPart::UNKNOWN:
                showScanPart = tr("δ֪��λ");
                break;
            case ScanPart::LIMBS:
                showScanPart = tr("��֫");
                break;
            case ScanPart::NECK:
                showScanPart = tr("����");
                break;
            case ScanPart::HEAD:
                showScanPart = tr("ͷ��");
                break;
            case ScanPart::TRUNCK:
                showScanPart = tr("����");
                break;
            case ScanPart::HEART:
                showScanPart = tr("����");
                break;
            case ScanPart::ABDOMEN:
                showScanPart = tr("����");
                break;
            case ScanPart::URINARY:
                showScanPart = tr("����ϵͳ");
                break;
            case ScanPart::SOFT_TISSUE:
                showScanPart = tr("����֯");
                break;
            case ScanPart::ENDOSCOPE:
                showScanPart = tr("�ڿ���");
                break;
            default:
                showScanPart = tr("δ֪��λ");
                break;
            }

            ui.QueryTableWidget->setItem(i, 4, new QTableWidgetItem(showScanPart));  //��鲿λ

            ui.QueryTableWidget->setItem(i, 5, new QTableWidgetItem(QString::fromLocal8Bit(m_pPatientsComplete[i].attending_doctor)));  //����ҽ��

            QString showSendMPPSStatus;
            if (m_pPatientsComplete[i].send_mpps_status)
            {
                showSendMPPSStatus = tr("�ѷ���");
            }
            else
            {
                showSendMPPSStatus = tr("δ����");
            }
            ui.QueryTableWidget->setItem(i, 6, new QTableWidgetItem(showSendMPPSStatus));  //MPPS״̬

            QString showStudy_date = QString::fromLocal8Bit(m_pPatientsComplete[i].study_date);
            if (!showStudy_date.isEmpty())
            {
                showStudy_date = showStudy_date.left(4) + '-' + showStudy_date.mid(4, 2) + '-' + showStudy_date.mid(6, 2);
            }
            QString showStudy_time = QString::fromLocal8Bit(m_pPatientsComplete[i].study_time);
            if (!showStudy_time.isEmpty())
            {
                showStudy_time = showStudy_time.left(2) + ':' + showStudy_time.mid(2, 2) + ':' + showStudy_time.mid(4, 2);
            }
            ui.QueryTableWidget->setItem(i, 7, new QTableWidgetItem(showStudy_date + " " + showStudy_time));  //���ʱ��

            ui.QueryTableWidget->item(i, 0)->setData(Qt::UserRole, QVariant::fromValue((void*)(m_pPatientsComplete + i)));  //�û��Զ�������
            ui.QueryTableWidget->setRowHeight(i, 42);//�����и�
        }
        setTableWidgetStyle();
        ui.QueryTableWidget->setSortingEnabled(true);
        //�л�����Ϣ��Ĭ��ѡ�е�һ��
        //ui.QueryTableWidget->selectRow(0);
        //ui.QueryTableWidget->setFocus();
        //setDataButtonStatus(true);
    }
    else
    {
        mQueryResultRemind->raise();
        mQueryResultRemind->show();
        setDataButtonStatus(false);
    }
}