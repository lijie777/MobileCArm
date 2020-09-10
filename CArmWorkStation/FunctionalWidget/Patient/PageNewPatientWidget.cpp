#include "PageNewPatientWidget.h"
#include "Common\CArmWorkStationCommon.h"
#include "CommonUtility.h"
#include <QListView>
#include <QtCore/QSettings.h>
#include "..\Message\MessageDialog.h"
#include "..\Message\MessageTipDialog.h"

PageNewPatientWidget::PageNewPatientWidget(CArmWorkStationProc* proc, QWidget *parent)
    : QFrame(parent)
    , moreInfoFrame(new QFrame(this))
    , orderInfoFrame(new QFrame(this))
    , WorkStationProc(proc)
{
    initUI();
    initLogic();
}

PageNewPatientWidget::~PageNewPatientWidget()
{

}

void PageNewPatientWidget::initUI()
{
    ui.setupUi(this);
    ui.MoreInfoLabel->installEventFilter(this);
    moreInfo_ui.setupUi(moreInfoFrame);
    ui.MoreInfoLayout->addWidget(moreInfoFrame);
    moreInfoFrame->hide();
    orderInfo_ui.setupUi(orderInfoFrame);
    ui.OrderInfoLayout->addWidget(orderInfoFrame);
    orderInfoFrame->hide();

    ui.GenderBox->setView(new QListView()); //设置下拉风格
    orderInfo_ui.ScanPartBox->setView(new QListView()); //设置下拉风格
                                                          //添加日历控件
    PatientBirthAction = new QAction(ui.BirthDateEdit);
    PatientBirthAction->setIcon(QIcon(":/CArmWorkStation/ImageSource/Patient/Doc_list_icon_calender.png"));
    ui.BirthDateEdit->addAction(PatientBirthAction, QLineEdit::TrailingPosition);

    StudyDateAction = new QAction(orderInfo_ui.ScheduledDateEdit);
    StudyDateAction->setIcon(QIcon(":/CArmWorkStation/ImageSource/Patient/Doc_list_icon_calender.png"));
    orderInfo_ui.ScheduledDateEdit->addAction(StudyDateAction, QLineEdit::TrailingPosition);

    if (currentPageMode != PageMode::EditSchedule)
    {
        setAutoPatinetID();
    }
}

void PageNewPatientWidget::initLogic()
{
    connect(this, SIGNAL(sigEmerPatient()), this, SLOT(toEmerPatient())); //新建急诊患者拍片
    connect(ui.ToScanButton, SIGNAL(clicked()), this, SLOT(showToScanDisplay()));  //显示新建拍片界面
    connect(ui.ToOrderButtton, SIGNAL(clicked()), this, SLOT(showToOrderDisplay())); //显示新建预约界面
    connect(ui.submitButton, SIGNAL(clicked()), this, SLOT(con_submit_clicked())); //填完信息后的确认按钮
    connect(ui.BirthDateEdit, SIGNAL(textChanged(QString)), this, SLOT(setAgeEdit(QString)));  //输入生日自动计算年龄
    connect(PatientBirthAction, SIGNAL(triggered()), this, SLOT(showPatientBirthCalendar()));     //显示日期控件
    connect(StudyDateAction, SIGNAL(triggered()), this, SLOT(showScheduledDateCalendar()));     //显示日期控件
    connect(this, SIGNAL(sigEditPatientScheduled(DB_Patient)), this, SLOT(toEditPatientScheduled(DB_Patient)));     //显示日期控件
}

void PageNewPatientWidget::toEmerPatient()
{
    MessageTipDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(tr("是否确定开始急诊患者拍片？"));
    int res = msgFrame.exec();

    if (res != QDialog::Accepted)
    {
        return;
    }

    std::string currentEmerNo = getTodayPatientEmerNo();
    std::string currenPatientEmerID = "EPID" + UCommonUtility::GetSystemTime().substr(0,8) + currentEmerNo;
    QString EmerName = tr("急诊") + QString::fromStdString(UCommonUtility::GetSystemTime().substr(0, 8)) + QString::fromStdString(currentEmerNo);
    DB_Patient mPatientEmer;
    mPatientEmer.is_emergency = true;
    strcpy(mPatientEmer.patient_id, currenPatientEmerID.c_str());
    strcpy(mPatientEmer.patient_first_name, EmerName.toLocal8Bit());

    //检测急诊患者ID是否存在
    bool bExist;
    ErrorCode errCheckPatientIDExistence = WorkStationProc->CheckPatientExistence(currenPatientEmerID, bExist);
    if (errCheckPatientIDExistence != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errCheckPatientIDExistence);
        remindMessage(tr("检查数据是否存在该患者ID失败！"));
        return;
    }

    if (!bExist)
    {
        remindMessage(tr("急诊患者ID冲突！"));
        return;
    }

    //存储该急诊患者到数据库
    ErrorCode errAddNewPatient = WorkStationProc->AddNewPatients(1, &mPatientEmer);
    if (errAddNewPatient != ErrorCode::ERR_SUCCESS)
    {
        remindMessage(tr("新增急诊患者失败"));
        return;
    }

    std::string tmpCompleteID;
    ErrorCode errAddPatientToComplete = WorkStationProc->AddPatientToComplete(mPatientEmer.patient_id, tmpCompleteID);
    if (errAddPatientToComplete != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errAddPatientToComplete);
        remindMessage(tr("新增急诊患者到已执行信息失败！"));
        LOG4CPLUS_ERROR(g_logger, L"Failed to add patient to complete");
        return;
    }

    //开始急诊患者拍片
    CArmWorkStationCommon::ToStartScan(mPatientEmer, tmpCompleteID);
}

void PageNewPatientWidget::toEditPatientScheduled(DB_Patient sPatient)
{
    noEdit_Patient = sPatient;
    //基本信息
    ui.LastNameEdit->setText(QString::fromLocal8Bit(sPatient.patient_last_name));
    ui.FirstNameEdit->setText(QString::fromLocal8Bit(sPatient.patient_first_name));
    ui.BirthDateEdit->setText(QString::fromLocal8Bit(sPatient.patient_birth_date));
    if (std::string(sPatient.patient_sex) == "M")
    {
        ui.GenderBox->setCurrentIndex(1);
    }
    else if (std::string(sPatient.patient_sex) == "F")
    {
        ui.GenderBox->setCurrentIndex(2);
    }
    else
    {
        ui.GenderBox->setCurrentIndex(0);
    }
    ui.PatientIDEdit->setText(QString::fromLocal8Bit(sPatient.patient_id));

    //更多信息
    moreInfo_ui.AessionEdit->setText(QString::fromLocal8Bit(sPatient.aession));
    moreInfo_ui.StudyIDEdit->setText(QString::fromLocal8Bit(sPatient.study_id));
    moreInfo_ui.HeightSpinBox->setValue(atoi(sPatient.patient_height));
    moreInfo_ui.WeightSpinBOx->setValue(atoi(sPatient.patient_weight));
    moreInfo_ui.ReferralDoctorEdit->setText(QString::fromLocal8Bit(sPatient.referral_doctor));
    moreInfo_ui.DescTextEdit->setText(QString::fromLocal8Bit(sPatient.patient_desc));
}

void PageNewPatientWidget::setAutoPatinetID()
{
    //自动生成患者ID，生成规则：就诊当天的年月日时分秒 +第几个病人
    std::string strPatientID = UCommonUtility::GetSystemTime() + getTodayPatientNo();
    ui.PatientIDEdit->setText(QString::fromStdString(strPatientID));
}

void PageNewPatientWidget::showToScanDisplay()
{
    clearWarning();
    clearInput();
    setPageMode(PageMode::NewScan);
    setAutoPatinetID();
}

void PageNewPatientWidget::showToOrderDisplay()
{
    clearWarning();
    clearInput();
    setPageMode(PageMode::NewOrder);
    setAutoPatinetID();
}

int PageNewPatientWidget::getCurrentAge(std::string str_birthDate)
{
    //日期不合规
    if (!UCommonUtility::CheckDate(&str_birthDate) || str_birthDate.empty())
    {
        return 0;
    }

    int birth_year;
    int birth_month;
    int birth_day;
    int study_year;
    int study_month;
    int study_day;
    int age;

    birth_year = std::atoi(str_birthDate.substr(0, 4).c_str());
    birth_month = std::atoi(str_birthDate.substr(4, 2).c_str());
    birth_day = std::atoi(str_birthDate.substr(6, 4).c_str());

    std::string currentDate = UCommonUtility::GetSystemTime();
    study_year = atoi(currentDate.substr(0, 4).c_str());
    study_month = atoi(currentDate.substr(4, 2).c_str());
    study_day = atoi(currentDate.substr(6, 2).c_str());


    if (birth_year == study_year)
        age = 0;
    else
    {
        age = study_year - birth_year;
        if (study_month<birth_month || (study_month == birth_month&&study_day<birth_day))
            age = age - 1;
    }
    return age;
}

void PageNewPatientWidget::setAgeEdit(QString text)
{
    ui.AgeEdit->setText(QString::number(getCurrentAge(text.toStdString())));
}

void PageNewPatientWidget::con_submit_clicked()
{
    //清空警告信息
    clearWarning();

    //新建拍片
    if (currentPageMode == PageMode::NewScan)
    {
        //保存并拍片
        SaveDataAndScan();
    }
    //新建预约
    else if (currentPageMode == PageMode::NewOrder)
    {
        //新建预约
        SaveData();
    }
    //编辑预约患者
    else if (currentPageMode == PageMode::EditSchedule)
    {
        //新建预约
        UpdateScheduleData();
    }
}

void PageNewPatientWidget::clearWarning()
{
    ui.LastNameWarningLabel->setText("");
    ui.FirstNameWarningLabel->setText("");
    ui.BirthDateWarningLabel->setText("");
    ui.AgeWarningLabel->setText("");
    ui.GenderWarningLabel->setText("");
    ui.PatientIDWarningLabel->setText("");

    moreInfo_ui.AessionWarningLabel->setText("");
    moreInfo_ui.StudyIDWarningLabel->setText("");
    moreInfo_ui.HeightWarningLabel->setText("");
    moreInfo_ui.WeightWarningLabel->setText("");
    moreInfo_ui.ReferralDoctorWarningLabel->setText("");
    moreInfo_ui.DescWarningLabel->setText("");

    orderInfo_ui.StudyDateWarningLabel->setText("");
    orderInfo_ui.AttendingDoctorWarningLabel->setText("");
    orderInfo_ui.SeriesTypeWarningLabel->setText("");
}

void PageNewPatientWidget::clearInput()
{
    ui.LastNameEdit->setText("");
    ui.FirstNameEdit->setText("");
    ui.BirthDateEdit->setText("");
    ui.AgeEdit->setText("");
    ui.GenderBox->setCurrentIndex(0);
    ui.PatientIDEdit->setText("");

    moreInfo_ui.AessionEdit->setText("");
    moreInfo_ui.StudyIDEdit->setText("");
    moreInfo_ui.HeightSpinBox->clear();
    moreInfo_ui.WeightSpinBOx->clear();
    moreInfo_ui.ReferralDoctorWarningLabel->setText("");
    moreInfo_ui.DescTextEdit->clear();

    orderInfo_ui.ScheduledDateEdit->setText("");
    orderInfo_ui.ScheduledHourSpinBox->clear();
    orderInfo_ui.ScheduledMinuteSpinBox->clear();
    orderInfo_ui.AttendingDoctorEdit->setText("");
    orderInfo_ui.ScanPartBox->setCurrentIndex(0);
}

void PageNewPatientWidget::SaveDataAndScan()
{
    if (inputCheck())
    {
        std::memset(&m_PatientInfo, 0, sizeof(DB_Patient));

        //调用数据库服务进程，写入数据到数据库
        std::strcpy(m_PatientInfo.patient_id, ui.PatientIDEdit->text().toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_first_name, ui.FirstNameEdit->text().toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_last_name, ui.LastNameEdit->text().toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_sex, strGender.toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_birth_date, strPatientBirth.c_str());
        std::strcpy(m_PatientInfo.patient_sex, strGender.toLocal8Bit());
        std::strcpy(m_PatientInfo.aession, strAession.toLocal8Bit());
        std::strcpy(m_PatientInfo.study_id, strStudyID.toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_height, strHeight.toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_weight, strWeight.toLocal8Bit());
        std::strcpy(m_PatientInfo.referral_doctor, strReferralDoctor.toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_desc, strDesc.toLocal8Bit());
        

        ErrorCode errAddNewPatient = WorkStationProc->AddNewPatients(1, &m_PatientInfo);
        if (errAddNewPatient != ErrorCode::ERR_SUCCESS)
        {
            //ErrorStatusRemind(errAddNewPatient);
            remindMessage(tr("新增患者失败！"));
            LOG4CPLUS_ERROR(g_logger, L"Failed to add patient");
            return;
        }
        remindMessage(tr("新增患者成功！"));

        std::string tmpCompleteID;
        ErrorCode errAddPatientToComplete = WorkStationProc->AddPatientToComplete(m_PatientInfo.patient_id, tmpCompleteID);
        if (errAddPatientToComplete != ErrorCode::ERR_SUCCESS)
        {
            //ErrorStatusRemind(errAddPatientToComplete);
            remindMessage(tr("新增患者到已执行信息失败！"));
            LOG4CPLUS_ERROR(g_logger, L"Failed to add patient to complete");
            return;
        }


        clearWarning();
        updateTodayPatientNo();
        CArmWorkStationCommon::ToStartScan(m_PatientInfo, tmpCompleteID);
    }
}

void PageNewPatientWidget::remindMessage(const QString & text)
{
    MessageDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(text);
    msgFrame.exec();
}

bool PageNewPatientWidget::checkString(QString qstr, int checkType)
{
    char *str = new char[256];
    char *tmp_str = str;
    std::strcpy(str, qstr.toLocal8Bit());
    //checkType==1 数字
    //checkType==2 字母、数字
    //checkType==3 中文、字母、数字
    //checkType==4 中文、字母
    bool result = false;
    bool isNumber = false;
    bool isLetter = false;
    bool isChinese = false;

    if (checkType == 1)
    {
        isNumber = true;
    }
    else if (checkType == 2)
    {
        isNumber = true;
        isLetter = true;
    }
    else if (checkType == 3)
    {
        isNumber = true;
        isLetter = true;
        isChinese = true;
    }
    else if (checkType == 4)
    {
        isLetter = true;
        isChinese = true;
    }


    while (*str)
    {
        if (((*str) & 0x80) && isChinese)  //中文
        {
            result = true;
            //++str;//汉字跳过一个字节检测（跳过会导致最后一个空格检测）
        }
        else if (((*str) >= 'a' && (*str) <= 'z' || (*str) >= 'A' && (*str) <= 'Z') && isLetter)  //字母
        {
            result = true;
        }
        else if (((*str) >= '0' && (*str) <= '9') && isNumber)   //数字
        {
            result = true;
        }
        else
        {
            result = false;
            break;
        }
        ++str;
    }

    delete tmp_str;

    //解决中文标点
    if (isChinese &&
        (qstr.contains(QStringLiteral("。"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("？"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("！"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("，"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("、"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("；"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("："), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("“"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("”"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("‘"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("’"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("（"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("）"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("——"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("……"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("—"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("·"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("《"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("》"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("〈"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("〉"), Qt::CaseSensitive) ||
            qstr.contains(QStringLiteral("____"), Qt::CaseSensitive)
            )
        )
    {
        result = false;
    }


    return result;
}

bool PageNewPatientWidget::inputCheck()
{
    //基本信息
    strPatientID = ui.PatientIDEdit->text();
    strLastName = ui.LastNameEdit->text();
    strFirstName = ui.FirstNameEdit->text();
    strGender = ui.GenderBox->currentText();
    strPatientBirth = ui.BirthDateEdit->text().toLocal8Bit();

    if (currentPageMode == PageMode::NewOrder)
    {
        //预约信息
        strScheduledDate = orderInfo_ui.ScheduledDateEdit->text().toLocal8Bit();
        strStudyHour = orderInfo_ui.ScheduledHourSpinBox->text();
        strStudyMinute = orderInfo_ui.ScheduledMinuteSpinBox->text();
        strAttendingDoctor = orderInfo_ui.AttendingDoctorEdit->text();
        mScanPart = (ScanPart)orderInfo_ui.ScanPartBox->currentIndex();
    }

    //更多信息
    strAession = moreInfo_ui.AessionEdit->text();
    strStudyID = moreInfo_ui.StudyIDEdit->text();
    strHeight = moreInfo_ui.HeightSpinBox->text();
    strWeight = moreInfo_ui.WeightSpinBOx->text();
    strReferralDoctor = moreInfo_ui.ReferralDoctorEdit->text();
    strDesc = moreInfo_ui.DescTextEdit->toPlainText();

    bool isCorrect = true;
    bool bExist = false;

    if (currentPageMode != PageMode::EditSchedule)
    {
        //基本信息
        //合法性检查
        //PatientID
        //判断是否输入为空
        if (strPatientID.isEmpty())
        {
            ui.PatientIDWarningLabel->setText(tr("请输入字母或数字，不超过32位！"));
            isCorrect = false;
        }
        //判断是否输入特殊字符
        else if (!checkString(strPatientID, 2))
        {
            ui.PatientIDWarningLabel->setText(tr("请输入字母或数字，不超过32位！"));
            isCorrect = false;
        }
        //输入长度应小于32个
        else if (strPatientID.length() > 32)
        {
            ui.PatientIDWarningLabel->setText(tr("请输入字母或数字，不超过32位！"));
            isCorrect = false;
        }
        //判断输入的患者ID是否已经存在
        else
        {
            //检测患者ID是否存在
            ErrorCode errCheckPatientIDExistence = WorkStationProc->CheckPatientExistence(strPatientID.toStdString(), bExist);
            if (errCheckPatientIDExistence != ErrorCode::ERR_SUCCESS)
            {
                //ErrorStatusRemind(errCheckPatientIDExistence);
                remindMessage(tr("检查数据是否存在该患者ID失败！"));
                return false;
            }

            if (!bExist)
            {
                ui.PatientIDWarningLabel->setText(tr("患者ID已存在！"));
                isCorrect = false;
            }
        }

        //LastName
        if (strLastName.isEmpty())
        {
            ui.LastNameWarningLabel->setText(tr("请输入中文或字母，不超过32位！"));
            isCorrect = false;
        }
        //判断是否输入特殊字符
        else if (!checkString(strLastName, 4))
        {
            ui.LastNameWarningLabel->setText(tr("请输入中文或字母，不超过32位！"));
            isCorrect = false;
        }
        //输入长度应小于32个
        else if (strLastName.length() > 32)
        {
            ui.LastNameWarningLabel->setText(tr("请输入中文或字母，不超过32位！"));
            isCorrect = false;
        }


        //FirstName
        if (strFirstName.isEmpty())
        {
            ui.FirstNameWarningLabel->setText(tr("请输入中文或字母，不超过32位！"));
            isCorrect = false;
        }
        //判断是否输入特殊字符
        else if (!checkString(strFirstName, 4))
        {
            ui.FirstNameWarningLabel->setText(tr("请输入中文或字母，不超过32位！"));
            isCorrect = false;
        }
        //输入长度应小于32个
        else if (strFirstName.length() > 32)
        {
            ui.FirstNameWarningLabel->setText(tr("请输入中文或字母，不超过32位！"));
            isCorrect = false;
        }

        //Gender
        if (strGender == "男")
            strGender = "M";
        else if (strGender == "女")
            strGender = "F";
        else
            strGender = "O";


        //PatientBirth
        if (strPatientBirth.empty())
        {
            ui.BirthDateWarningLabel->setText(tr("出生信息不能为空！"));
            isCorrect = false;
        }
        //判断是否输入格式为YYYY-MM-DD、YYYY/MM/DD、YYYYMMDD
        else if (!UCommonUtility::CheckDate(&strPatientBirth))
        {
            ui.BirthDateWarningLabel->setText(tr("请输入正确日期格式"));
            isCorrect = false;
        }
        else if (strPatientBirth < "18000101" || strPatientBirth > UCommonUtility::GetSystemTime().substr(0,8))
        {
            ui.BirthDateWarningLabel->setText(tr("范围：1800-01-01至当前日期"));
            isCorrect = false;
        }
    }
   
    //更多信息
    //Aession#
    if (!strAession.isEmpty())
    {
        //判断是否输入特殊字符
        if (!checkString(strAession, 2))
        {
            moreInfo_ui.AessionWarningLabel->setText(tr("请输入字母、数字，不超过32位！"));
            isCorrect = false;
        }
        //输入长度应小于32个
        else if (strAession.length() > 32)
        {
            moreInfo_ui.AessionWarningLabel->setText(tr("请输入字母、数字，不超过32位！"));
            isCorrect = false;
        }
    }

    //Study ID
    if (!strStudyID.isEmpty())
    {
        //判断是否输入特殊字符
        if (!checkString(strStudyID, 1))
        {
            moreInfo_ui.StudyIDWarningLabel->setText(tr("请输入数字，不超过16位！"));
            isCorrect = false;
        }
        //输入长度应小于32个
        else if (strStudyID.length() > 16)
        {
            moreInfo_ui.StudyIDWarningLabel->setText(tr("请输入数字，不超过16位！"));
            isCorrect = false;
        }
    }
    
    //诊断医生
    if (!strReferralDoctor.isEmpty())
    {
        //判断是否输入特殊字符
        if (!checkString(strReferralDoctor, 4))
        {
            moreInfo_ui.ReferralDoctorWarningLabel->setText(tr("请输入中文、字母、数字，不超过32位！"));
            isCorrect = false;
        }
        //输入长度应小于32个
        else if (strReferralDoctor.length() > 32)
        {
            moreInfo_ui.ReferralDoctorWarningLabel->setText(tr("请输入中文、字母、数字，不超过32位！"));
            isCorrect = false;
        }
    }

    //评论描述
    if (!strDesc.isEmpty())
    {
        if (strDesc.length() > 255)
        {
            moreInfo_ui.ReferralDoctorWarningLabel->setText(tr("最大长度不超过255位！"));
            isCorrect = false;
        }
    }

    //预约信息
    if (currentPageMode == PageMode::NewOrder)
    {
        //检查日期
        if (strScheduledDate.empty())
        {
            orderInfo_ui.StudyDateWarningLabel->setText(tr("检查日期不能为空！"));
            isCorrect = false;
        }
        //判断是否输入格式为YYYY-MM-DD、YYYY/MM/DD、YYYYMMDD
        else if (!UCommonUtility::CheckDate(&strScheduledDate))
        {
            orderInfo_ui.StudyDateWarningLabel->setText(tr("请输入正确日期格式"));
            isCorrect = false;
        }
        else if (strScheduledDate < UCommonUtility::GetSystemTime().substr(0, 8) || strScheduledDate>"99990101")
        {
            orderInfo_ui.StudyDateWarningLabel->setText(tr("范围：当前日期 至 9999-01-01"));
            isCorrect = false;
        }

        //主治医生
        if (strAttendingDoctor.isEmpty())
        {
            orderInfo_ui.AttendingDoctorWarningLabel->setText(tr("请输入中文或字母，不超过32位！"));
            isCorrect = false;
        }
        //判断是否输入特殊字符
        else if (!checkString(strAttendingDoctor, 4))
        {
            orderInfo_ui.AttendingDoctorWarningLabel->setText(tr("请输入中文或字母，不超过32位！"));
            isCorrect = false;
        }
        //输入长度应小于32个
        else if (strAttendingDoctor.length() > 32)
        {
            orderInfo_ui.AttendingDoctorWarningLabel->setText(tr("请输入中文或字母，不超过32位！"));
            isCorrect = false;
        }

        //检查拍摄
        if (mScanPart == ScanPart::UNKNOWN)
        {
            orderInfo_ui.SeriesTypeWarningLabel->setText(tr("请选择一个拍摄类型！"));
            isCorrect = false;
        }
    }
    return isCorrect;
}

void PageNewPatientWidget::SaveData()
{
    if (inputCheck())
    {
        std::memset(&m_PatientSchedule, 0, sizeof(DB_PatientSchedule));
        
        //调用数据库服务进程，写入数据到数据库
        std::strcpy(m_PatientSchedule.mDB_Patient.patient_id, ui.PatientIDEdit->text().toLocal8Bit());
        std::strcpy(m_PatientSchedule.mDB_Patient.patient_first_name, ui.FirstNameEdit->text().toLocal8Bit());
        std::strcpy(m_PatientSchedule.mDB_Patient.patient_last_name, ui.LastNameEdit->text().toLocal8Bit());
        std::strcpy(m_PatientSchedule.mDB_Patient.patient_sex, strGender.toLocal8Bit());
        std::strcpy(m_PatientSchedule.mDB_Patient.patient_birth_date, strPatientBirth.c_str());
        std::strcpy(m_PatientSchedule.mDB_Patient.patient_sex, strGender.toLocal8Bit());
        std::strcpy(m_PatientSchedule.mDB_Patient.aession, strAession.toLocal8Bit());
        std::strcpy(m_PatientSchedule.mDB_Patient.study_id, strStudyID.toLocal8Bit());
        std::strcpy(m_PatientSchedule.mDB_Patient.patient_height, strHeight.toLocal8Bit());
        std::strcpy(m_PatientSchedule.mDB_Patient.patient_weight, strWeight.toLocal8Bit());
        std::strcpy(m_PatientSchedule.mDB_Patient.referral_doctor, strReferralDoctor.toLocal8Bit());
        std::strcpy(m_PatientSchedule.mDB_Patient.patient_desc, strDesc.toLocal8Bit());

        std::strcpy(m_PatientSchedule.scheduled_date, strScheduledDate.c_str());

        QString strStudyTime;
        if (strStudyHour.isEmpty())
        {
            strStudyTime += "00";
        }
        else if (strStudyHour.length() == 1)
        {
            strStudyTime = strStudyTime + "0" + strStudyHour;
        }
        else
        {
            strStudyTime += strStudyHour;
        }

        if (strStudyMinute.isEmpty())
        {
            strStudyTime += "00";
        }
        else if (strStudyMinute.length() == 1)
        {
            strStudyTime = strStudyTime + "0" + strStudyMinute;
        }
        else
        {
            strStudyTime += strStudyMinute;
        }

        strStudyTime += "00";

        std::strcpy(m_PatientSchedule.scheduled_time, strStudyTime.toLocal8Bit());
        std::strcpy(m_PatientSchedule.attending_doctor, strAttendingDoctor.toLocal8Bit());
        m_PatientSchedule.scan_part = (int)mScanPart;

        ErrorCode errAddNewPatient = WorkStationProc->AddNewPatientsSchedule(1, &m_PatientSchedule);
        if (errAddNewPatient != ErrorCode::ERR_SUCCESS)
        {
            //ErrorStatusRemind(errAddNewPatient);
            remindMessage(tr("新增预约失败！"));
            LOG4CPLUS_ERROR(g_logger, L"Failed to add patient schedule");
            return;
        }
        remindMessage(tr("新增预约成功！"));
        clearWarning();
        updateTodayPatientNo();
    }
}

void PageNewPatientWidget::UpdateScheduleData()
{
    if (inputCheck())
    {
        std::memset(&m_PatientInfo, 0, sizeof(DB_Patient));

        //调用数据库服务进程，写入数据到数据库
        std::strcpy(m_PatientInfo.patient_id, ui.PatientIDEdit->text().toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_first_name, ui.FirstNameEdit->text().toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_last_name, ui.LastNameEdit->text().toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_sex, strGender.toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_birth_date, strPatientBirth.c_str());
        std::strcpy(m_PatientInfo.patient_sex, strGender.toLocal8Bit());
        std::strcpy(m_PatientInfo.aession, strAession.toLocal8Bit());
        std::strcpy(m_PatientInfo.study_id, strStudyID.toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_height, strHeight.toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_weight, strWeight.toLocal8Bit());
        std::strcpy(m_PatientInfo.referral_doctor, strReferralDoctor.toLocal8Bit());
        std::strcpy(m_PatientInfo.patient_desc, strDesc.toLocal8Bit());

        if (isDataChange(noEdit_Patient, m_PatientInfo))
        {
            remindMessage(tr("患者信息没有任何更改！"));
            clearWarning();
            return;
        }

        ErrorCode errAddNewPatient = WorkStationProc->UpdatePatient(m_PatientInfo.patient_id, &m_PatientInfo);
        if (errAddNewPatient != ErrorCode::ERR_SUCCESS)
        {
            //ErrorStatusRemind(errAddNewPatient);
            remindMessage(tr("更新患者失败！"));
            LOG4CPLUS_ERROR(g_logger, L"Failed to update patient");
            return;
        }
        remindMessage(tr("更新患者成功！"));
        clearWarning();
    }
}

bool PageNewPatientWidget::isDataChange(DB_Patient oldPatient, DB_Patient newPatient)
{
    //更多信息
    if (strcmp(oldPatient.aession, newPatient.aession) != 0)
    {
        return true;
    }
    if (strcmp(oldPatient.study_id, newPatient.study_id) != 0)
    {
        return true;
    }
    if (strcmp(oldPatient.patient_height, newPatient.patient_height) != 0)
    {
        return true;
    }
    if (strcmp(oldPatient.patient_weight, newPatient.patient_weight) != 0)
    {
        return true;
    }
    if (strcmp(oldPatient.referral_doctor, newPatient.referral_doctor) != 0)
    {
        return true;
    }
    if (strcmp(oldPatient.patient_desc, newPatient.patient_desc) != 0)
    {
        return true;
    }

    //患者基本信息
    if (strcmp(oldPatient.patient_inst_id, newPatient.patient_inst_id) != 0)
    {
    return true;
    }
    if (strcmp(oldPatient.patient_id, newPatient.patient_id) != 0)
    {
    return true;
    }
    if (strcmp(oldPatient.patient_first_name, newPatient.patient_first_name) != 0)
    {
    return true;
    }
    if (strcmp(oldPatient.patient_last_name, newPatient.patient_last_name) != 0)
    {
    return true;
    }
    if (strcmp(oldPatient.patient_birth_date, newPatient.patient_birth_date) != 0)
    {
    return true;
    }
    if (strcmp(oldPatient.patient_birth_time, newPatient.patient_birth_time) != 0)
    {
    return true;
    }
    if (strcmp(oldPatient.patient_sex, newPatient.patient_sex) != 0)
    {
    return true;
    }
    if (strcmp(oldPatient.pregnancy_status, newPatient.pregnancy_status) != 0)
    {
    return true;
    }
    if (strcmp(oldPatient.telephone_number, newPatient.telephone_number) != 0)
    {
    return true;
    }
    if (strcmp(oldPatient.id_card_number, newPatient.id_card_number) != 0)
    {
    return true;
    }
    
    return false;
}

void PageNewPatientWidget::showPatientBirthCalendar()
{
    MyCalendarWidget* PatientBirthCalendar = new MyCalendarWidget(ui.BirthDateEdit, ui.BirthDateEdit, ":/CArmWorkStation/ImageSource/Patient");
    PatientBirthCalendar->move(ui.BirthDateEdit->mapToGlobal(ui.BirthDateEdit->pos()).x(), ui.BirthDateEdit->mapToGlobal(ui.BirthDateEdit->pos()).y());
    PatientBirthCalendar->show();
}

void PageNewPatientWidget::showScheduledDateCalendar()
{
    MyCalendarWidget* ScheduledDateCalendar = new MyCalendarWidget(orderInfo_ui.ScheduledDateEdit, orderInfo_ui.ScheduledDateEdit, ":/CArmWorkStation/ImageSource/Patient");
    ScheduledDateCalendar->setMinMaxDate(QDate::currentDate(), QDate(9999, 1, 1));
    ScheduledDateCalendar->move(orderInfo_ui.ScheduledDateEdit->mapToGlobal(orderInfo_ui.ScheduledDateEdit->pos()).x(), orderInfo_ui.ScheduledDateEdit->mapToGlobal(orderInfo_ui.ScheduledDateEdit->pos()).y());
    ScheduledDateCalendar->show();
}

bool PageNewPatientWidget::eventFilter(QObject * obj, QEvent * e)
{
    if (obj == ui.MoreInfoLabel && e->type() == QEvent::MouseButtonPress)
    {
        if (!moreInfoFrame->isVisible())
        {
            moreInfoFrame->show();
        }
        else
        {
            moreInfoFrame->hide();
        }
        return true;
    }
    return false;
}

std::string PageNewPatientWidget::getTodayPatientEmerNo()
{
    //取当前配置文件中记录的编号，如果日期不是今天，则取1
    QSettings cfg("TodayPatientEmerNo.ini", QSettings::IniFormat); //今天的日期yyyymmdd
    int intToday = cfg.value("Today").toInt();
    int intEmerNo = cfg.value("EmerNo").toInt();
    int currentDate = atoi(UCommonUtility::GetSystemTime().substr(0, 8).c_str());
    if (intToday == 0 || currentDate != intToday)
    {
        intEmerNo = 1;
    }
    intToday = currentDate;
    cfg.setValue("Today", intToday);
    int nextEmerNo = intEmerNo + 1;
    cfg.setValue("EmerNo", nextEmerNo);
    cfg.sync();

    char strEmerNo[10];
    sprintf(strEmerNo, "%03d", intEmerNo);
    return strEmerNo;
}

std::string PageNewPatientWidget::getTodayPatientNo()
{
    //取当前配置文件中记录的编号，如果日期不是今天，则取1
    QSettings cfg("TodayPatientNo.ini", QSettings::IniFormat);
    int intToday = cfg.value("Today").toInt();
    int intNo = cfg.value("No").toInt();
    int currentDate = atoi(UCommonUtility::GetSystemTime().substr(0, 8).c_str()); //今天的日期yyyymmdd
    if (intToday == 0 || currentDate != intToday)
    {
        intNo = 1;
    }
    char strNo[10];
    sprintf(strNo, "%04d", intNo);
    return strNo;
}

void PageNewPatientWidget::updateTodayPatientNo()
{
    //更新配置文件中的记录，只有保存患者成功才会更新今天是第几个患者
    QSettings cfg("TodayPatientNo.ini", QSettings::IniFormat);
    int intToday = cfg.value("Today").toInt();
    int intNo = cfg.value("No").toInt();
    int currentDate = atoi(UCommonUtility::GetSystemTime().substr(0, 8).c_str()); //今天的日期yyyymmdd
    if (intToday == 0 || currentDate != intToday)
    {
        intNo = 1;
    }
    else
    {
        intNo++;
    }

    intToday = currentDate;
    cfg.setValue("Today", intToday);
    cfg.setValue("No", intNo);
    cfg.sync();
}

void PageNewPatientWidget::setPageMode(PageMode mode)
{
    currentPageMode = mode;
    if (mode == PageMode::NewScan)
    {
        ui.NewPatientSelectWidget->show();
        orderInfoFrame->hide();
        moreInfoFrame->hide();
        ui.submitButton->setText(tr("新建拍片"));
    }
    else  if (mode == PageMode::NewOrder)
    {
        ui.NewPatientSelectWidget->show();
        orderInfoFrame->show();
        moreInfoFrame->hide();
        ui.submitButton->setText(tr("新建"));
    }
    else if (mode == PageMode::EditSchedule)
    {
        ui.NewPatientSelectWidget->hide();
        orderInfoFrame->hide();
        moreInfoFrame->show();
        ui.BaseInfoWidget->setEnabled(false);
        ui.submitButton->setText(tr("确认更改"));
    }

}