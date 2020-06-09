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
    //清空查询结果数据
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

    ui.GenderBox->setView(new QListView()); //设置下拉风格
    ui.SourceBox->setView(new QListView()); //设置下拉风格

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
    ui.QueryTableWidget->setColumnWidth(0, 70);   //编号
    ui.QueryTableWidget->setColumnWidth(1, 130); //来源
    ui.QueryTableWidget->setColumnWidth(2, 115); //姓名
    ui.QueryTableWidget->setColumnWidth(3, 200); //病人ID
    ui.QueryTableWidget->setColumnWidth(4, 105); //检查部位
    ui.QueryTableWidget->setColumnWidth(5, 120); //主治医生

    mQueryResultRemind->setFixedWidth(ui.QueryTableWidget->width());
    QueryResultRemind_ui->RemindLabel->setFixedWidth(ui.QueryTableWidget->width());
    mQueryResultRemind->move(0, 150);
    mQueryResultRemind->hide();
}

void PageScheduledWidget::initLogic()
{
    connect(ui.ClearButton, SIGNAL(clicked()), this, SLOT(con_ClearButton_clicked()));  //点击“清空”按钮
    connect(ui.QueryButton, SIGNAL(clicked()), this, SLOT(con_QueryButton_clicked()));  //点击“查询”按钮
    connect(ui.QueryTableWidget, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(setDataButtonEnable()));     //点击“患者信息”Widget的某行数据
    connect(ui.QueryTableWidget, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(setDataButtonEnable()));     //键盘移动
    connect(SchedueldDateBeginAction, SIGNAL(triggered()), this, SLOT(showScheduledDateBeginCalendar()));     //显示日期控件
    connect(ScheduledDateEndAction, SIGNAL(triggered()), this, SLOT(showScheduledDateEndCalendar()));     //显示日期控件
    connect(ui.DeleteButton, SIGNAL(clicked()), this, SLOT(con_DeleteButton_clicked()));  //点击“删除”按钮
    connect(ui.EditButton, SIGNAL(clicked()), this, SLOT(con_EditButton_clicked()));  //点击“编辑”按钮
    connect(ui.HistoryButton, SIGNAL(clicked()), this, SIGNAL(sigPatientScheduledHistory()));  //点击“预约历史”按钮
    connect(ui.ReturnButton, SIGNAL(clicked()), this, SIGNAL(sigPatientScheduled()));  //点击“返回”按钮
    connect(ui.SubmitButton, SIGNAL(clicked()), this, SLOT(con_SubmitButton_clicked()));  //点击“确定”按钮
}

void PageScheduledWidget::con_SubmitButton_clicked()
{
    MessageTipDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(tr("您确定要开始拍片吗？"));
    int res = msgFrame.exec();

    if (res != QDialog::Accepted)
    {
        return;
    }

    std::string strPatientCompleteID;
    ErrorCode errAddPatientScheduleToComplete = WorkStationProc->AddPatientScheduleToComplete(currentPatientSchedule->patient_schedule_id, strPatientCompleteID);
    if (errAddPatientScheduleToComplete != ErrorCode::ERR_SUCCESS)
    {
        remindMessage(tr("新增预约信息到已执行信息失败！"));
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
    msgFrame.message_ui.msgLabel->setText(tr("您确定要删除该患者预约信息吗？"));
    int res = msgFrame.exec();

    if (res != QDialog::Accepted)
    {
        return;
    }

    ErrorCode errDeletePatientsSchedule = WorkStationProc->DeletePatientsSchedule(currentPatientSchedule->patient_schedule_id);
    if (errDeletePatientsSchedule != ErrorCode::ERR_SUCCESS)
    {
        remindMessage(tr("删除患者预约信息失败！"));
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
            ui.QueryTableWidget->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter); //设置显示居中
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

    //性别判断
    if (ui.GenderBox->currentText() == "男")
        strcpy(mSearchCondition.patient_sex, "M");
    else if (ui.GenderBox->currentText() == "女")
        strcpy(mSearchCondition.patient_sex, "F");
    else
        strcpy(mSearchCondition.patient_sex, "");

    strcpy(mSearchCondition.doctor_name, ui.DoctorEdit->text().toLocal8Bit());

    std::string strScheduledDateBegin = ui.ScheduledDateBeginEdit->text().toLocal8Bit();
    std::string strScheduledDateEnd = ui.ScheduledDateEndEdit->text().toLocal8Bit();

    //判断是否输入格式为YYYY-MM-DD、YYYY/MM/DD、YYYYMMDD(统一转换成YYYYMMDD)
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
    //预约历史查询界面
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
    

    //输入错误直接查询为空
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

    //调用数据库进行查询
    m_nPatientCount = 0;
    m_pPatientsSchedule = nullptr;

    ErrorCode errGetLocalPatientsSchedule = WorkStationProc->GetLocalPatientsSchedule(mSearchCondition, m_nPatientCount, &m_pPatientsSchedule);
    if (errGetLocalPatientsSchedule != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errGetLocalPatientsSchedule);
        remindMessage(tr("查询数据库失败！"));
        return;
    }

    if (m_nPatientCount > 0)
    {
        ui.QueryTableWidget->setSortingEnabled(false);
        //显示数据
        for (int i = 0; i < m_nPatientCount; i++)
        {
            ui.QueryTableWidget->insertRow(i);
            ui.QueryTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));  //编号
            
            ui.QueryTableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromLocal8Bit("")));  //来源

            std::string showPatientName = tostring(m_pPatientsSchedule[i].mDB_Patient.patient_last_name) 
                + tostring(m_pPatientsSchedule[i].mDB_Patient.patient_first_name);
            ui.QueryTableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromLocal8Bit(showPatientName.c_str())));  //姓名

            ui.QueryTableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromLocal8Bit(m_pPatientsSchedule[i].mDB_Patient.patient_id)));  //病人ID

            ScanPart tmpScanPart = (ScanPart)m_pPatientsSchedule[i].scan_part;
            QString showScanPart;
            switch (tmpScanPart)
            {
            case ScanPart::UNKNOWN:
                showScanPart = tr("未知部位");
                break;
            case ScanPart::LIMBS:
                showScanPart = tr("四肢");
                break;
            case ScanPart::NECK:
                showScanPart = tr("颈部");
                break;
            case ScanPart::HEAD:
                showScanPart = tr("头部");
                break;
            case ScanPart::TRUNCK:
                showScanPart = tr("躯干");
                break;
            case ScanPart::HEART:
                showScanPart = tr("心脏");
                break;
            case ScanPart::ABDOMEN:
                showScanPart = tr("腹部");
                break;
            case ScanPart::URINARY:
                showScanPart = tr("泌尿系统");
                break;
            case ScanPart::SOFT_TISSUE:
                showScanPart = tr("软组织");
                break;
            case ScanPart::ENDOSCOPE:
                showScanPart = tr("内窥镜");
                break;
            default:
                showScanPart = tr("未知部位");
                break;
            }

            ui.QueryTableWidget->setItem(i, 4, new QTableWidgetItem(showScanPart));  //检查部位

            ui.QueryTableWidget->setItem(i, 5, new QTableWidgetItem(QString::fromLocal8Bit(m_pPatientsSchedule[i].attending_doctor)));  //主治医生

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
            ui.QueryTableWidget->setItem(i, 6, new QTableWidgetItem(showScheduled_date + " " + showScheduled_time));  //预约时间

            ui.QueryTableWidget->item(i, 0)->setData(Qt::UserRole, QVariant::fromValue((void*)(m_pPatientsSchedule + i)));  //用户自定义数据
            ui.QueryTableWidget->setRowHeight(i, 42);//设置行高
        }
        setTableWidgetStyle();
        ui.QueryTableWidget->setSortingEnabled(true);
        //有患者信息则默认选中第一行
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