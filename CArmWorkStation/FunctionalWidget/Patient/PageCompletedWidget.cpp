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
    //清空查询结果数据
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

    ui.GenderBox->setView(new QListView()); //设置下拉风格
    ui.SourceBox->setView(new QListView()); //设置下拉风格

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
    ui.QueryTableWidget->setColumnWidth(0,65);   //编号
    ui.QueryTableWidget->setColumnWidth(1, 95); //检查状态
    ui.QueryTableWidget->setColumnWidth(2, 105); //姓名
    ui.QueryTableWidget->setColumnWidth(3, 180); //病人ID
    ui.QueryTableWidget->setColumnWidth(4, 105); //检查部位
    ui.QueryTableWidget->setColumnWidth(5, 100); //主治医生
    ui.QueryTableWidget->setColumnWidth(6, 115); //MPPS状态

    mQueryResultRemind->setFixedWidth(ui.QueryTableWidget->width());
    QueryResultRemind_ui->RemindLabel->setFixedWidth(ui.QueryTableWidget->width());
    mQueryResultRemind->move(0, 150);
    mQueryResultRemind->hide();
}

void PageCompletedWidget::initLogic()
{
    connect(ui.ClearButton, SIGNAL(clicked()), this, SLOT(con_ClearButton_clicked()));  //点击“清空”按钮
    connect(ui.QueryButton, SIGNAL(clicked()), this, SLOT(con_QueryButton_clicked()));  //点击“查询”按钮
    connect(ui.QueryTableWidget, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(setDataButtonEnable()));     //点击“患者信息”Widget的某行数据
    connect(ui.QueryTableWidget, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(setDataButtonEnable()));     //键盘移动
    connect(StudyDateBeginAction, SIGNAL(triggered()), this, SLOT(showScheduledDateBeginCalendar()));     //显示日期控件
    connect(StudyDateEndAction, SIGNAL(triggered()), this, SLOT(showScheduledDateEndCalendar()));     //显示日期控件
    connect(ui.DeleteButton, SIGNAL(clicked()), this, SLOT(con_DeleteButton_clicked()));  //点击“删除”按钮
    connect(ui.EditButton, SIGNAL(clicked()), this, SLOT(con_EditButton_clicked()));  //点击“编辑”按钮
    connect(ui.NewStudyButton, SIGNAL(clicked()), this, SLOT(con_NewStudyButton_clicked()));  //点击“新检查”按钮
    connect(ui.RecoverStudyButton, SIGNAL(clicked()), this, SLOT(con_RecoverStudyButton_clicked()));  //点击“恢复检查”按钮
    connect(ui.StudyButton, SIGNAL(clicked()), this, SLOT(con_StudyButton_clicked()));  //点击“查看检查”按钮
}

void PageCompletedWidget::con_NewStudyButton_clicked()
{
    MessageTipDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(tr("您确定要新建检查吗？"));
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
        remindMessage(tr("新建检查失败！"));
        LOG4CPLUS_ERROR(g_logger, L"Failed to add patient to complete");
        return;
    }
    CArmWorkStationCommon::ToStartScan(currentPatientComplete->mDB_Patient, tmpCompleteID);
    con_QueryButton_clicked();
}

void PageCompletedWidget::con_RecoverStudyButton_clicked()
{
    MessageTipDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(tr("您确定要恢复检查吗？"));
    int res = msgFrame.exec();

    if (res != QDialog::Accepted)
    {
        return;
    }

    ErrorCode errUpdatePatientCompleteStatus = WorkStationProc->UpdatePatientCompleteStatus(currentPatientComplete->patient_complete_id, StudyStatus::Doing);
    if (errUpdatePatientCompleteStatus != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errAddPatientToComplete);
        remindMessage(tr("恢复检查失败！"));
        LOG4CPLUS_ERROR(g_logger, L"Failed to update patient complete status");
        return;
    }

    ui.QueryTableWidget->setItem(cRow, 1, new QTableWidgetItem(tr("进行中")));  //检查状态
    ui.QueryTableWidget->item(cRow, 1)->setTextColor(QColor(0, 145, 255)); //蓝色进行中

    //重新开始拍片
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
    msgFrame.message_ui.msgLabel->setText(tr("您确定要删除本次检查吗？"));
    int res = msgFrame.exec();

    if (res != QDialog::Accepted)
    {
        return;
    }

    ErrorCode errDeletePatientsSchedule = WorkStationProc->DeletePatientsComplete(currentPatientComplete->patient_complete_id);
    if (errDeletePatientsSchedule != ErrorCode::ERR_SUCCESS)
    {
        remindMessage(tr("删除检查信息失败！"));
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
        //只有已中断的可以“恢复检查”
        if (currentStudyStatus == StudyStatus::Interrupt)
        {
            ui.RecoverStudyButton->setEnabled(true);
        }
        else
        {
            ui.RecoverStudyButton->setEnabled(false);
        }

        //只有已中断、已完成的可以“删除”、“新检查”
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
        //只有已完成的可以“查看检查”
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

    //性别判断
    if (ui.GenderBox->currentText() == "男")
        strcpy(mSearchCondition.patient_sex, "M");
    else if (ui.GenderBox->currentText() == "女")
        strcpy(mSearchCondition.patient_sex, "F");
    else
        strcpy(mSearchCondition.patient_sex, "");

    strcpy(mSearchCondition.doctor_name, ui.DoctorEdit->text().toLocal8Bit());

    std::string strStudyDateBegin = ui.StudyDateBeginEdit->text().toLocal8Bit();
    std::string strStudyDateEnd = ui.StudyDateEndEdit->text().toLocal8Bit();

    //判断是否输入格式为YYYY-MM-DD、YYYY/MM/DD、YYYYMMDD(统一转换成YYYYMMDD)
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

    //输入错误直接查询为空
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

    //调用数据库进行查询
    m_nPatientCount = 0;
    m_pPatientsComplete = nullptr;

    ErrorCode errGetLocalPatientsComplete = WorkStationProc->GetLocalPatientsComplete(mSearchCondition, m_nPatientCount, &m_pPatientsComplete);
    if (errGetLocalPatientsComplete != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errGetLocalPatientsComplete);
        remindMessage(tr("查询数据库失败！"));
        return;
    }
    
    if (m_nPatientCount > 0)
    {
        ui.QueryTableWidget->setSortingEnabled(false);
        //显示数据
        for (int i = 0; i < m_nPatientCount; i++)
        {
            std::string showPatientName = tostring(m_pPatientsComplete[i].mDB_Patient.patient_last_name) + tostring(m_pPatientsComplete[i].mDB_Patient.patient_first_name);
            ui.QueryTableWidget->insertRow(i);
            ui.QueryTableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));  //编号 

            QString showStudyStatus;
            if (m_pPatientsComplete[i].study_status == StudyStatus::Doing)
            {
                showStudyStatus = tr("进行中");
                ui.QueryTableWidget->setItem(i, 1, new QTableWidgetItem(showStudyStatus));  //检查状态
                ui.QueryTableWidget->item(i, 1)->setTextColor(QColor(0, 145, 255)); //蓝色进行中
            }
            else if (m_pPatientsComplete[i].study_status == StudyStatus::Interrupt)
            {
                showStudyStatus = tr("已中断");
                ui.QueryTableWidget->setItem(i, 1, new QTableWidgetItem(showStudyStatus));  //检查状态
                ui.QueryTableWidget->item(i, 1)->setTextColor(QColor(252, 57, 35)); //红色已中断
            }
            else if (m_pPatientsComplete[i].study_status == StudyStatus::Completed)
            {
                showStudyStatus = tr("已完成");
                ui.QueryTableWidget->setItem(i, 1, new QTableWidgetItem(showStudyStatus));  //检查状态
                ui.QueryTableWidget->item(i, 1)->setTextColor(QColor(180, 196, 217)); //默认色已完成
            }

            ui.QueryTableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromLocal8Bit(showPatientName.c_str())));  //姓名

            ui.QueryTableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromLocal8Bit(m_pPatientsComplete[i].mDB_Patient.patient_id)));  //病人ID

            ScanPart tmpScanPart = (ScanPart)m_pPatientsComplete[i].scan_part;
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

            ui.QueryTableWidget->setItem(i, 5, new QTableWidgetItem(QString::fromLocal8Bit(m_pPatientsComplete[i].attending_doctor)));  //主治医生

            QString showSendMPPSStatus;
            if (m_pPatientsComplete[i].send_mpps_status)
            {
                showSendMPPSStatus = tr("已发送");
            }
            else
            {
                showSendMPPSStatus = tr("未发送");
            }
            ui.QueryTableWidget->setItem(i, 6, new QTableWidgetItem(showSendMPPSStatus));  //MPPS状态

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
            ui.QueryTableWidget->setItem(i, 7, new QTableWidgetItem(showStudy_date + " " + showStudy_time));  //检查时间

            ui.QueryTableWidget->item(i, 0)->setData(Qt::UserRole, QVariant::fromValue((void*)(m_pPatientsComplete + i)));  //用户自定义数据
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