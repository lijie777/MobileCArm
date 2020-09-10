#include "PageScheduledWidget.h"
#include "Common\CArmWorkStationCommon.h"
#include "CommonUtility.h"
#include <QListView>
#include <QtCore/QSettings.h>
#include "..\Message\MessageDialog.h"
#include "..\Message\MessageTipDialog.h"


PageScheduledWidget::PageScheduledWidget(CArmWorkStationProc* proc, bool isHistory, QWidget *parent)
    : QFrame(parent)
    , WorkStationProc(proc)
    , QueryResultRemind_ui(new Ui::RemindFrame)
    , isScheduledHistoryMode(isHistory)
{
    initUI();
    initLogic();

    m_nPatientCount = 0;
    m_pPatientsSchedule = nullptr;
    currentPatientSchedule = nullptr;
}

PageScheduledWidget::~PageScheduledWidget()
{
    //��ղ�ѯ�������
    m_nPatientCount = 0;
    if (m_pPatientsSchedule != nullptr)
    {
        delete[] m_pPatientsSchedule;
        m_pPatientsSchedule = nullptr;
    }
}

void PageScheduledWidget::initUI()
{
    ui.setupUi(this);

    if (!isScheduledHistoryMode)
    {
        ui.ReturnButtonWidget->hide();
    }
    else
    {
        ui.OperateButtonWidget->hide();
    }

    ui.GenderBox->setView(new QListView()); //�����������
    ui.SourceBox->setView(new QListView()); //�����������

    SchedueldDateBeginAction = new QAction(ui.ScheduledDateBeginEdit);
    ScheduledDateEndAction = new QAction(ui.ScheduledDateEndEdit);
    SchedueldDateBeginAction->setIcon(QIcon(":/CArmWorkStation/ImageSource/Patient/Doc_list_icon_calender.png"));
    ScheduledDateEndAction->setIcon(QIcon(":/CArmWorkStation/ImageSource/Patient/Doc_list_icon_calender.png"));
    ui.ScheduledDateBeginEdit->addAction(SchedueldDateBeginAction, QLineEdit::TrailingPosition);
    ui.ScheduledDateEndEdit->addAction(ScheduledDateEndAction, QLineEdit::TrailingPosition);

    mQueryResultRemind = new QFrame(ui.QueryTableWidget);
    QueryResultRemind_ui->setupUi(mQueryResultRemind);
    ui.QueryTableWidget->horizontalHeader()->setHighlightSections(false);
    ui.QueryTableWidget->setFocusPolicy(Qt::NoFocus);
    ui.QueryTableWidget->setColumnWidth(0, 70);   //���
    ui.QueryTableWidget->setColumnWidth(1, 130); //��Դ
    ui.QueryTableWidget->setColumnWidth(2, 115); //����
    ui.QueryTableWidget->setColumnWidth(3, 200); //����ID
    ui.QueryTableWidget->setColumnWidth(4, 105); //��鲿λ
    ui.QueryTableWidget->setColumnWidth(5, 120); //����ҽ��

    mQueryResultRemind->setFixedWidth(ui.QueryTableWidget->width());
    QueryResultRemind_ui->RemindLabel->setFixedWidth(ui.QueryTableWidget->width());
    mQueryResultRemind->move(0, 150);
    mQueryResultRemind->hide();
}

void PageScheduledWidget::initLogic()
{
    connect(ui.ClearButton, SIGNAL(clicked()), this, SLOT(con_ClearButton_clicked()));  //�������ա���ť
    connect(ui.QueryButton, SIGNAL(clicked()), this, SLOT(con_QueryButton_clicked()));  //�������ѯ����ť
    connect(ui.QueryTableWidget, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(setDataButtonEnable()));     //�����������Ϣ��Widget��ĳ������
    connect(ui.QueryTableWidget, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(setDataButtonEnable()));     //�����ƶ�
    connect(SchedueldDateBeginAction, SIGNAL(triggered()), this, SLOT(showScheduledDateBeginCalendar()));     //��ʾ���ڿؼ�
    connect(ScheduledDateEndAction, SIGNAL(triggered()), this, SLOT(showScheduledDateEndCalendar()));     //��ʾ���ڿؼ�
    connect(ui.DeleteButton, SIGNAL(clicked()), this, SLOT(con_DeleteButton_clicked()));  //�����ɾ������ť
    connect(ui.EditButton, SIGNAL(clicked()), this, SLOT(con_EditButton_clicked()));  //������༭����ť
    connect(ui.HistoryButton, SIGNAL(clicked()), this, SIGNAL(sigPatientScheduledHistory()));  //�����ԤԼ��ʷ����ť
    connect(ui.ReturnButton, SIGNAL(clicked()), this, SIGNAL(sigPatientScheduled()));  //��������ء���ť
    connect(ui.SubmitButton, SIGNAL(clicked()), this, SLOT(con_SubmitButton_clicked()));  //�����ȷ������ť
}

void PageScheduledWidget::con_SubmitButton_clicked()
{
    MessageTipDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(tr("��ȷ��Ҫ��ʼ��Ƭ��"));
    int res = msgFrame.exec();

    if (res != QDialog::Accepted)
    {
        return;
    }

    std::string strPatientCompleteID;
    ErrorCode errAddPatientScheduleToComplete = WorkStationProc->AddPatientScheduleToComplete(currentPatientSchedule->patient_schedule_id, strPatientCompleteID);
    if (errAddPatientScheduleToComplete != ErrorCode::ERR_SUCCESS)
    {
        remindMessage(tr("����ԤԼ��Ϣ����ִ����Ϣʧ�ܣ�"));
        return;
    }
    ui.QueryTableWidget->removeRow(cRow);
    CArmWorkStationCommon::ToStartScan(currentPatientSchedule->mDB_Patient, strPatientCompleteID);
}

void PageScheduledWidget::con_EditButton_clicked()
{
    emit sigEditPatientScheduled(currentPatientSchedule->mDB_Patient);
}

void PageScheduledWidget::remindMessage(const QString & text)
{
    MessageDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(text);
    msgFrame.exec();
}

void PageScheduledWidget::con_ClearButton_clicked()
{
    clearCondition();
    clearRemind();
    setDataButtonStatus(false);
}

void PageScheduledWidget::con_DeleteButton_clicked()
{
    MessageTipDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(tr("��ȷ��Ҫɾ���û���ԤԼ��Ϣ��"));
    int res = msgFrame.exec();

    if (res != QDialog::Accepted)
    {
        return;
    }

    ErrorCode errDeletePatientsSchedule = WorkStationProc->DeletePatientsSchedule(currentPatientSchedule->patient_schedule_id);
    if (errDeletePatientsSchedule != ErrorCode::ERR_SUCCESS)
    {
        remindMessage(tr("ɾ������ԤԼ��Ϣʧ�ܣ�"));
    }
    ui.QueryTableWidget->removeRow(cRow);
}

void PageScheduledWidget::clearCondition()
{
    ui.PatientNameEdit->clear();
    ui.PatientIDEdit->clear();
    ui.GenderBox->setCurrentIndex(0);
    ui.DoctorEdit->clear();
    ui.ScheduledDateBeginEdit->clear();
    ui.ScheduledDateEndEdit->clear();
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

void PageScheduledWidget::clearRemind()
{
    ui.QueryTableWidget->clearContents();
    ui.QueryTableWidget->setRowCount(0);
    mQueryResultRemind->hide();
}

void PageScheduledWidget::getCurrentRowInfo()
{
    cRow = ui.QueryTableWidget->currentRow();
    if (cRow < 0)
        return;
    currentPatientSchedule = (DB_PatientSchedule *)ui.QueryTableWidget->item(cRow, 0)->data(Qt::UserRole).value<void*>();
}

void PageScheduledWidget::setDataButtonStatus(bool m_status)
{
    getCurrentRowInfo();
    ui.DeleteButton->setEnabled(m_status);
    ui.EditButton->setEnabled(m_status);
    ui.SubmitButton->setEnabled(m_status);
}

void PageScheduledWidget::setTableWidgetStyle()
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

void PageScheduledWidget::setDataButtonDisable()
{
    setDataButtonStatus(false);
}

void PageScheduledWidget::setDataButtonEnable()
{
    setDataButtonStatus(true);
}

void PageScheduledWidget::showScheduledDateBeginCalendar()
{
    MyCalendarWidget* ScheduledDateBeginCalendar = new MyCalendarWidget(ui.ScheduledDateBeginEdit, ui.ScheduledDateBeginEdit, ":/CArmWorkStation/ImageSource/Patient");
    if (!isScheduledHistoryMode)
    {
        ScheduledDateBeginCalendar->setMinMaxDate(QDate::currentDate(), QDate(9999, 1, 1));
    }
    else
    {
        ScheduledDateBeginCalendar->setMinMaxDate(QDate(1800, 1, 1), QDate::currentDate().addDays(-1));
    }
    ScheduledDateBeginCalendar->move(this->mapToGlobal(ui.ScheduledDateBeginEdit->pos()).x(), this->mapToGlobal(ui.ScheduledDateBeginEdit->pos()).y() + ui.ScheduledDateBeginEdit->geometry().height());
    ScheduledDateBeginCalendar->show();
}

void PageScheduledWidget::showScheduledDateEndCalendar()
{
    MyCalendarWidget* ScheduledDateEndCalendar = new MyCalendarWidget(ui.ScheduledDateEndEdit, ui.ScheduledDateEndEdit, ":/CArmWorkStation/ImageSource/Patient");
    if (!isScheduledHistoryMode)
    {
        ScheduledDateEndCalendar->setMinMaxDate(QDate::currentDate(), QDate(9999, 1, 1));
    }
    else
    {
        ScheduledDateEndCalendar->setMinMaxDate(QDate(1800, 1, 1), QDate::currentDate().addDays(-1));
    }
    ScheduledDateEndCalendar->move(this->mapToGlobal(ui.ScheduledDateEndEdit->pos()).x(), this->mapToGlobal(ui.ScheduledDateEndEdit->pos()).y() + ui.ScheduledDateEndEdit->geometry().height());
    ScheduledDateEndCalendar->show();
}

void PageScheduledWidget::con_QueryButton_clicked()
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

    std::string strScheduledDateBegin = ui.ScheduledDateBeginEdit->text().toLocal8Bit();
    std::string strScheduledDateEnd = ui.ScheduledDateEndEdit->text().toLocal8Bit();

    //�ж��Ƿ������ʽΪYYYY-MM-DD��YYYY/MM/DD��YYYYMMDD(ͳһת����YYYYMMDD)
    bool isInputError = false;
    if (!isScheduledHistoryMode)
    {
        if (strScheduledDateBegin.empty())
        {
            strScheduledDateBegin = UCommonUtility::GetSystemTime().substr(0, 8);
            std::string showDate = UCommonUtility::GetSystemTime().substr(0, 4) + "-" + UCommonUtility::GetSystemTime().substr(4, 2) + "-" + UCommonUtility::GetSystemTime().substr(6, 2);
            ui.ScheduledDateBeginEdit->setText(QString::fromStdString(showDate));
        }
        else
        {
            if (!UCommonUtility::CheckDate(&strScheduledDateBegin))
            {
                isInputError = true;
            }
            else if (strScheduledDateBegin < UCommonUtility::GetSystemTime().substr(0, 8))
            {
                strScheduledDateBegin = UCommonUtility::GetSystemTime().substr(0, 8);
                std::string showDate = UCommonUtility::GetSystemTime().substr(0, 4) + "-" + UCommonUtility::GetSystemTime().substr(4, 2) + "-" + UCommonUtility::GetSystemTime().substr(6, 2);
                ui.ScheduledDateBeginEdit->setText(QString::fromStdString(showDate));
            }
            else if (strScheduledDateBegin > "99990101")
            {
                strScheduledDateBegin = "99990101";
                ui.ScheduledDateBeginEdit->setText("9999-01-01");
            }
        }

        if (!strScheduledDateEnd.empty())
        {
            if (!UCommonUtility::CheckDate(&strScheduledDateEnd))
            {
                isInputError = true;
            }
            else if (strScheduledDateEnd < UCommonUtility::GetSystemTime().substr(0, 8))
            {
                strScheduledDateEnd = UCommonUtility::GetSystemTime().substr(0, 8);
                std::string showDate = UCommonUtility::GetSystemTime().substr(0, 4) + "-" + UCommonUtility::GetSystemTime().substr(4, 2) + "-" + UCommonUtility::GetSystemTime().substr(6, 2);
                ui.ScheduledDateEndEdit->setText(QString::fromStdString(showDate));
            }
            else if (strScheduledDateEnd > "99990101")
            {
                strScheduledDateEnd = "99990101";
                ui.ScheduledDateEndEdit->setText("9999-01-01");
            }
        }
    }
    //ԤԼ��ʷ��ѯ����
    else
    {
        mSearchCondition.is_scheduled_history = true;
        if (!strScheduledDateBegin.empty())
        {
            if (!UCommonUtility::CheckDate(&strScheduledDateBegin))
            {
                isInputError = true;
            }
            else if (strScheduledDateBegin >= UCommonUtility::GetSystemTime().substr(0, 8))
            {
                strScheduledDateBegin = QDate::currentDate().addDays(-1).toString("yyyyMMdd").toStdString();
                std::string showDate = QDate::currentDate().addDays(-1).toString("yyyy-MM-dd").toStdString();;
                ui.ScheduledDateBeginEdit->setText(QString::fromStdString(showDate));
            }
            else if (strScheduledDateBegin < "18000101")
            {
                strScheduledDateBegin = "18000101";
                ui.ScheduledDateBeginEdit->setText("1800-01-01");
            }
        }

        if (strScheduledDateEnd.empty())
        {
            strScheduledDateEnd = QDate::currentDate().addDays(-1).toString("yyyyMMdd").toStdString();
            std::string showDate = QDate::currentDate().addDays(-1).toString("yyyy-MM-dd").toStdString();
            ui.ScheduledDateEndEdit->setText(QString::fromStdString(showDate));
        }
        else
        {
            if (!UCommonUtility::CheckDate(&strScheduledDateEnd))
            {
                isInputError = true;
            }
            else if (strScheduledDateEnd >= UCommonUtility::GetSystemTime().substr(0, 8))
            {
                strScheduledDateEnd = QDate::currentDate().addDays(-1).toString("yyyyMMdd").toStdString();
                std::string showDate = QDate::currentDate().addDays(-1).toString("yyyy-MM-dd").toStdString();
                ui.ScheduledDateEndEdit->setText(QString::fromStdString(showDate));
            }
            else if (strScheduledDateEnd < "18000101")
            {
                strScheduledDateEnd = "18000101";
                ui.ScheduledDateEndEdit->setText("1800-01-01");
            }
        }
    }
    

    //�������ֱ�Ӳ�ѯΪ��
    if (isInputError)
    {
        mQueryResultRemind->show();
        setDataButtonStatus(false);
        return;
    }

    strcpy(mSearchCondition.scheduled_date_begin, strScheduledDateBegin.c_str());
    strcpy(mSearchCondition.scheduled_date_end, strScheduledDateEnd.c_str());
    
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
    m_pPatientsSchedule = nullptr;

    ErrorCode errGetLocalPatientsSchedule = WorkStationProc->GetLocalPatientsSchedule(mSearchCondition, m_nPatientCount, &m_pPatientsSchedule);
    if (errGetLocalPatientsSchedule != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errGetLocalPatientsSchedule);
        remindMessage(tr("��ѯ���ݿ�ʧ�ܣ�"));
        return;
    }

    if (m_nPatientCount > 0)
    {
        ui.QueryTableWidget->setSortingEnabled(false);
        //��ʾ����
        for (int i = 0; i < m_nPatientCount; i++)
        {
            ui.QueryTableWidget->insertRow(i);
            ui.QueryTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));  //���
            
            ui.QueryTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromLocal8Bit("")));  //��Դ

            std::string showPatientName = tostring(m_pPatientsSchedule[i].mDB_Patient.patient_last_name) 
                + tostring(m_pPatientsSchedule[i].mDB_Patient.patient_first_name);
            ui.QueryTableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromLocal8Bit(showPatientName.c_str())));  //����

            ui.QueryTableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromLocal8Bit(m_pPatientsSchedule[i].mDB_Patient.patient_id)));  //����ID

            ScanPart tmpScanPart = (ScanPart)m_pPatientsSchedule[i].scan_part;
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

            ui.QueryTableWidget->setItem(i, 5, new QTableWidgetItem(QString::fromLocal8Bit(m_pPatientsSchedule[i].attending_doctor)));  //����ҽ��

            QString showScheduled_date = QString::fromLocal8Bit(m_pPatientsSchedule[i].scheduled_date);
            if (!showScheduled_date.isEmpty())
            {
                showScheduled_date = showScheduled_date.left(4) + '-' + showScheduled_date.mid(4, 2) + '-' + showScheduled_date.mid(6, 2);
            }
            QString showScheduled_time = QString::fromLocal8Bit(m_pPatientsSchedule[i].scheduled_time);
            if (!showScheduled_time.isEmpty())
            {
                showScheduled_time = showScheduled_time.left(2) + ':' + showScheduled_time.mid(2, 2) + ':' + showScheduled_time.mid(4, 2);
            }
            ui.QueryTableWidget->setItem(i, 6, new QTableWidgetItem(showScheduled_date + " " + showScheduled_time));  //ԤԼʱ��

            ui.QueryTableWidget->item(i, 0)->setData(Qt::UserRole, QVariant::fromValue((void*)(m_pPatientsSchedule + i)));  //�û��Զ�������
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