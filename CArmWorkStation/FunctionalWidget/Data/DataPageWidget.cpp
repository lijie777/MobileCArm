#include <QListView>
#include "DataPageWidget.h"
#include "..\Message\MessageDialog.h"
#include "..\Message\MessageTipDialog.h"

DataPageWidget::DataPageWidget(CArmWorkStationProc* proc, QWidget *parent)
    : QFrame(parent)
    , WorkStationProc(proc)
    , PatientStatusRemind_ui(new Ui::DataPageRemindWidget)
{
    initUI();
    initLogic();
    currentTmpSeries = nullptr;
    countTmpSeries = 0;
}

DataPageWidget::~DataPageWidget()
{
    //清空查询结果数据
    countTmpSeries = 0;
    if (currentTmpSeries != nullptr)
    {
        delete[] currentTmpSeries;
        currentTmpSeries = nullptr;
    }
}

void DataPageWidget::initUI()
{
    ui.setupUi(this);
    mPatientStatusRemind = new QFrame(ui.scrollAreaWidgetContents);
    PatientStatusRemind_ui->setupUi(mPatientStatusRemind);
    mPatientStatusRemind->setFixedWidth(ui.scrollAreaWidgetContents->width());
    PatientStatusRemind_ui->RemindLabel->setFixedWidth(ui.scrollAreaWidgetContents->width());
    mPatientStatusRemind->move(0, 150);
    mPatientStatusRemind->hide();
    ui.StudyDateBox->setView(new QListView()); //设置下拉风格
    ui.StudyTimeBox->setView(new QListView()); //设置下拉风格
}

void DataPageWidget::initLogic()
{
    connect(this, SIGNAL(sigStudyImage(DB_PatientComplete)), this, SLOT(startStudyImage(DB_PatientComplete)));  //查看检查
    connect(this, SIGNAL(sigClickDataBtn()), this, SLOT(showCurrentData()));  //点击“图像目录”按钮
    connect(ui.StudyDateBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(con_StudyDateBox_change(const QString &)));  //选择“检查日期”按钮
    connect(ui.StudyTimeBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(con_StudyTimeBox_change(const QString &)));  //选择“检查时间”按钮
}

void DataPageWidget::con_StudyDateBox_change(const QString & text)
{
    if (text.isEmpty() || !isAllowStudyDate)
        return;
    QString strStudyDate;
    strStudyDate = text.left(4) + text.mid(5, 2) + text.mid(8, 2);
    setStudyTimeBox(strStudyDate.toStdString());
}

void DataPageWidget::setStudyTimeBox(const std::string & selStudyDate)
{
    ui.StudyTimeBox->clear();
    mapStudyTime.clear();

    for (auto it : vecDBStudyDate)
    {
        if (std::string(it.study_date) == selStudyDate)
        {
            mapStudyTime[it.study_time] = it.study_instance_uid;
        }
    }
    //增加studytime下拉框
    isAllowStudyTime = false;
    int j = 0;
    for (auto it : mapStudyTime)
    {
        QString showStudyTime = QString::fromLocal8Bit(it.first.c_str());
        if (!showStudyTime.isEmpty())
        {
            showStudyTime = showStudyTime.left(2) + ':' + showStudyTime.mid(2, 2) + ':' + showStudyTime.mid(4, 2);
        }
        ui.StudyTimeBox->insertItem(j, showStudyTime);
        if (std::string(it.first) == currentStudyTime && isDefaultStudyDate)
        {
            ui.StudyTimeBox->setCurrentIndex(j);
        }
        j++;
    }
    isAllowStudyTime = true;

    con_StudyTimeBox_change(ui.StudyTimeBox->currentText());
}

void DataPageWidget::con_StudyTimeBox_change(const QString & text)
{
    if (text.isEmpty() || !isAllowStudyTime)
        return;
    QString selStudyTime = text;
    selStudyTime = selStudyTime.left(2) + selStudyTime.mid(3, 2) + selStudyTime.mid(6, 2);

    auto iter = mapStudyTime.find(selStudyTime.toStdString());
    std::string studyUID = iter->second;

    countTmpSeries = 0;
    currentTmpSeries = nullptr;
    //查询当前StudyUID的拍摄信息
    ErrorCode errGetTmpSeriesByStudy = WorkStationProc->GetTmpSeriesByStudy(studyUID, countTmpSeries, &currentTmpSeries);
    if (errGetTmpSeriesByStudy != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errAddPatientToComplete);
        remindMessage(tr("查询拍摄信息失败！"));
        return;
    }
    if (countTmpSeries == 0)
        return;

    showPreviewDisplay();
}

void DataPageWidget::showCurrentData()
{
    if (!isLoadPatient)
    {
        clearAllDisplay();
        mPatientStatusRemind->show();
    }
}

void DataPageWidget::clearPreviewDisplay()
{
    SelectedCount = 0;
    ui.SelectInfoLabel->setText(tr("已选择0张图像"));     //选择信息
    mPatientStatusRemind->hide();
    for (auto val : vePreviewWidget)
    {
        delete val;
        val = NULL;
    }
    vePreviewWidget.clear();

    QLayoutItem *child;
    while ((child = ui.verticalLayout->takeAt(0)) != 0)
    {
        //setParent为NULL，防止删除之后界面不消失
        if (child->widget())
        {
            child->widget()->setParent(NULL);
        }

        delete child;
    }
}

void DataPageWidget::clearAllDisplay()
{
    ui.PatientNameLabel->setText("");  //姓名
    ui.PatientSexLabel->setText("");  //性别
    ui.PatientAgeLabel->setText("");  //年龄
    ui.PatientIDLabel->setText("");  //患者ID
    ui.PatientBirthLabel->setText("");  //出生日期
    ui.DoctorLabel->setText("");  //诊断医生
    ui.DescLabel->setText("");  //描述
    ui.StudyDateBox->clear();   //检查日期
    ui.StudyTimeBox->clear();   //检查时间
    clearPreviewDisplay();
}

int DataPageWidget::getCurrentAge(std::string str_birthDate)
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

void DataPageWidget::remindMessage(const QString & text)
{
    MessageDialog msgFrame;
    msgFrame.message_ui.msgLabel->setText(text);
    msgFrame.exec();
}

void DataPageWidget::startStudyImage(DB_PatientComplete sPatientComplete)
{
    clearAllDisplay();
    currentPatient = sPatientComplete.mDB_Patient;
    currentPatientCompleteID = sPatientComplete.patient_complete_id;
    currentStudyDate = sPatientComplete.study_date;
    currentStudyTime = sPatientComplete.study_time;
    isLoadPatient = true;

    QString showPatientName = QString::fromLocal8Bit(currentPatient.patient_last_name) + QString::fromLocal8Bit(currentPatient.patient_first_name);
    ui.PatientNameLabel->setText(showPatientName);  //姓名

    QString showGender = QString::fromLocal8Bit(currentPatient.patient_sex);

    if (showGender == "M" || showGender == "m")
    {
        showGender = "男";
    }
    else if (showGender == "F" || showGender == "f")
    {
        showGender = "女";
    }
    else
    {
        showGender = "其他";
    }
    ui.PatientSexLabel->setText(showGender);  //性别

    int currentAge = getCurrentAge(currentStudyDate);
    QString showAge = QString::number(currentAge) + tr("周岁");
    ui.PatientAgeLabel->setText(showAge);  //年龄

    ui.PatientIDLabel->setText(currentPatient.patient_id);  //患者ID

    QString showPatientBirthDate = QString::fromLocal8Bit(currentPatient.patient_birth_date);
    if (!showPatientBirthDate.isEmpty())
    {
        showPatientBirthDate = showPatientBirthDate.left(4) + '-' + showPatientBirthDate.mid(4, 2) + '-' + showPatientBirthDate.mid(6, 2);
    }
    ui.PatientBirthLabel->setText(showPatientBirthDate);  //出生日期

    ui.DoctorLabel->setText(QString::fromLocal8Bit(currentPatient.referral_doctor));  //诊断医生
    ui.DescLabel->setText(QString::fromLocal8Bit(currentPatient.patient_desc));  //描述

    countTmpSeries = 0;
    currentTmpSeries = nullptr;
    //查询当前患者执行ID的拍摄信息
    ErrorCode errGetTmpSeriesByComplete = WorkStationProc->GetTmpSeriesByComplete(currentPatientCompleteID, countTmpSeries, &currentTmpSeries);
    if (errGetTmpSeriesByComplete != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errAddPatientToComplete);
        remindMessage(tr("查询拍摄信息失败！"));
        return;
    }
    if (countTmpSeries == 0)
        return;
    
    showPreviewDisplay();
    setStudyDateBox();
}

void DataPageWidget::showPreviewDisplay()
{
    clearPreviewDisplay();
    for (int i = 0; i < countTmpSeries; i++)
    {
        PreviewWidget *mPreviewWidget = new PreviewWidget(i + 1);
        connect(mPreviewWidget, SIGNAL(sigPreviewChecked(bool)), this, SLOT(PreviewChecked(bool)));
        connect(mPreviewWidget, SIGNAL(sigPreviewDoubleClicked(std::string)), this, SLOT(PreviewDoubleClicked(std::string)));
        connect(mPreviewWidget, SIGNAL(sigFoldButtonChecked(PreviewWidget *, bool)), this, SLOT(FoldButtonChecked(PreviewWidget *, bool)));
        mPreviewWidget->updateDisplay(currentTmpSeries[i]);
        QHBoxLayout *hLayout = new QHBoxLayout();
        QSpacerItem *hSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);  //horizon 
        ui.verticalLayout->addLayout(hLayout);
        hLayout->addItem(hSpacer);
        mPreviewWidget->BelongHLayout = hLayout;
        hLayout->insertWidget(0, mPreviewWidget);
        mPreviewWidget->show();
        vePreviewWidget.push_back(mPreviewWidget);
    }
    QSpacerItem *vSpacer = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding);   //vertical
    ui.verticalLayout->addItem(vSpacer);
}

void DataPageWidget::setStudyDateBox()
{
    vecDBStudyDate.clear();

    int countDBStudyDate = 0;

    DB_StudyDate *mDBStudyDate;
    //查询当前患者ID的所有拍摄日期时间信息
    ErrorCode errGetTmpStudyDateByPatient = WorkStationProc->GetTmpStudyDateByPatient(currentPatient.patient_id, countDBStudyDate, &mDBStudyDate);
    if (errGetTmpStudyDateByPatient != ErrorCode::ERR_SUCCESS)
    {
        //ErrorStatusRemind(errAddPatientToComplete);
        remindMessage(tr("查询所有拍摄日期时间信息失败！"));
        return;
    }
    if (countDBStudyDate == 0)
        return;

    std::map<std::string, int> mapStudyDate;
    for (int i = 0; i < countDBStudyDate; i++)
    {
        vecDBStudyDate.push_back(mDBStudyDate[i]);

        auto iter = mapStudyDate.find(mDBStudyDate[i].study_date);
        if (iter == mapStudyDate.end()) mapStudyDate[mDBStudyDate[i].study_date] = mDBStudyDate[i].study_time_cnt;
    }
    
    delete[] mDBStudyDate;
    mDBStudyDate = nullptr;


    //增加studydate下拉框
    isAllowStudyDate = false;
    int j = 0;
    for (auto it: mapStudyDate)
    {
        QString showStudyDate = QString::fromLocal8Bit(it.first.c_str());
        if (!showStudyDate.isEmpty())
        {
            showStudyDate = showStudyDate.left(4) + '-' + showStudyDate.mid(4, 2) + '-' + showStudyDate.mid(6, 2);
        }
        ui.StudyDateBox->insertItem(j, showStudyDate);
        if (isDefaultStudyDate = (std::string(it.first) == currentStudyDate))
        {
            ui.StudyDateBox->setCurrentIndex(j);
        }
        j++;
    }
    isAllowStudyDate = true;

    setStudyTimeBox(currentStudyDate);
}

void DataPageWidget::PreviewChecked(bool ischecked)
{
    if (ischecked)
    {
        SelectedCount++;
    }
    else
    {
        SelectedCount--;
    }
    ui.SelectInfoLabel->setText(tr("已选择") + QString::number(SelectedCount) + tr("张图像"));
}

void DataPageWidget::FoldButtonChecked(PreviewWidget * view, bool ischecked)
{

}

void DataPageWidget::PreviewDoubleClicked(std::string dicomPath)
{
    //发起阅片
}
