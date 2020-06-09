#include "PreviewWidget.h"
#include<iostream>
#include "QDir"

PreviewWidget::PreviewWidget(int num, QFrame *parent) :
    QFrame(parent)
    , ui(new Ui::PreviewWidget)
    , m_clickedTimer(new QTimer(this))
    , PreviewNumber(num)
{
    initUI();
    initLogic();
    this->installEventFilter(this);     //�¼�������ע��
    ui->ImageCountLabel->installEventFilter(this);     //�¼�������ע��
}

PreviewWidget::~PreviewWidget()
{
}

void PreviewWidget::initUI()
{
    ui->setupUi(this);
}

void PreviewWidget::initLogic()
{
    connect(m_clickedTimer, SIGNAL(timeout()), this, SLOT(slotSingleClicked()));
    connect(ui->SelectCheckBox, SIGNAL(clicked()), this, SIGNAL(sigPreviewClicked()));
    connect(ui->ImageCountLabel, SIGNAL(clicked()), this, SLOT(con_ImageCountLabel_clicked()));
    connect(ui->FoldCheckBox, SIGNAL(clicked()), this, SLOT(con_FoldCheckBox_clicked()));
}

void PreviewWidget::con_ImageCountLabel_clicked()
{
    ui->FoldCheckBox->setChecked(!ui->FoldCheckBox->isChecked());
    con_FoldCheckBox_clicked();
}

void PreviewWidget::con_FoldCheckBox_clicked()
{
    if (ui->FoldCheckBox->isChecked())
    {
        ui->ImageCountLabel->hide();
    }
    else
    {
        ui->ImageCountLabel->show();
    }
    emit sigFoldButtonChecked(this, ui->FoldCheckBox->isChecked());
}

void PreviewWidget::updateDisplay(DB_TmpSeries mTmp_Series, QString filepath)
{
    //�Ƿ��ѱ���
    if (mTmp_Series.isSave)
    {
        ui->DeleteRemindLabel->hide();
        ui->SaveButton->hide();
    }
    else
    {
        //�Ȱ�Ĭ��72Сʱ����ʱ�����
        int remainHour = 71;
        int remainMinute= 59;
        //��������ʱ�����ʣ�ౣ��ʱ��
        ui->DeleteRemindLabel->setText(QString::number(remainHour) + tr("Сʱ") + QString::number(remainMinute) + tr("��ɾ��"));
    }

    //ͼ����
    ui->ImageNumberLabel->setText(QString::number(PreviewNumber));

    //���ͼ��
    if (atoi(mTmp_Series.mDB_Series.images_in_acquisition) == 1)
    {
        ui->FoldCheckBox->hide();
        ui->ImageCountLabel->hide();
    }
    else 
    {
        ui->FoldCheckBox->show();
        ui->ImageCountLabel->setText(QString::fromLocal8Bit(mTmp_Series.mDB_Series.images_in_acquisition));
    }

    QString imagePath; //չʾ��ͼ��ȫ·��
    if (filepath.isEmpty())
    {
        std::stringstream sDICOMImgDir;
        sDICOMImgDir << mTmp_Series.mDB_Series.root_path << '\\' << mTmp_Series.mDB_Series.series_dir;
        strDicomPath = sDICOMImgDir.str();
        QString strPreview = QString::fromStdString(sDICOMImgDir.str()) + "\\" + "preview"; //Ԥ���ļ���

        QDir dir(strPreview);
        QStringList nameFilters;
        //�����ļ����˸�ʽ
        nameFilters << "*.png";
        //�����˺���ļ����ƴ��뵽files�б���
        QStringList list = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
        if (list.size() == 0)
            return;
        foreach(QString str, list)
        {
            imagePath = strPreview + '/' + str;
            break;
        }
    }
    else
    {
        imagePath = filepath;
        strDicomPath = filepath.toLocal8Bit().replace(filepath.toLocal8Bit().length() - 3, 3, "prj");
        QFileInfo fileInfo(QString::fromLocal8Bit(strDicomPath.c_str()));
        //��δ���ɲ�����¼��ʹ��ԭƬ
        if (!fileInfo.exists())
        {
            std::stringstream sDICOMImgDir;
            sDICOMImgDir << mTmp_Series.mDB_Series.root_path << '\\' << mTmp_Series.mDB_Series.series_dir;
            strDicomPath = sDICOMImgDir.str();
            QString strPreview = QString::fromStdString(sDICOMImgDir.str()) + "\\" + "preview"; //Ԥ���ļ���

            QDir dir(strPreview);
            QStringList nameFilters;
            //�����ļ����˸�ʽ
            nameFilters << "*.png";
            //�����˺���ļ����ƴ��뵽files�б���
            QStringList list = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
            if (list.size() == 0)
                return;
            foreach(QString str, list)
            {
                imagePath = strPreview + '/' + str;
                break;
            }
        }
    }

    QPixmap img_pix;
    img_pix.load(imagePath);
    int new_height = this->height() - 2;
    int new_width = 1.0*img_pix.width()*new_height / img_pix.height();
    if (new_width > this->width() - 2)
    {
        new_width = this->width() - 2;
        new_height = 1.0*img_pix.height()*new_width / img_pix.width();
    }

    img_pix = img_pix.scaled(new_width, new_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->ImageLabel->setAlignment(Qt::AlignCenter);
    ui->ImageLabel->setPixmap(img_pix);
}

void PreviewWidget::changePreviewNumber(int num)
{
    PreviewNumber = num;
    //ͼ����
    ui->ImageNumberLabel->setText(QString::number(PreviewNumber));
}

bool PreviewWidget::eventFilter(QObject *mObject, QEvent *mEvent)
{
    if (mObject == ui->ImageCountLabel && mEvent->type() == QEvent::MouseButtonPress)
    {
        return true;
    }
    else if (mObject == this && mEvent->type() == QEvent::Enter)
    {
        setPreviewWidgetHover();
    }
    else if (mObject == this && mEvent->type() == QEvent::Leave)
    {
        setPreviewWidgetDefault();
    }
    return false;
}

void PreviewWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (m_clickedTimer->isActive())
        m_clickedTimer->stop();
    emit sigPreviewDoubleClicked(strDicomPath);
}

void PreviewWidget::mousePressEvent(QMouseEvent *e)
{
    if (Qt::LeftButton == e->button())
    {
        m_clickedTimer->start(300);
    }
}

void PreviewWidget::slotSingleClicked()
{
    if (m_clickedTimer->isActive())
        m_clickedTimer->stop();
    ui->SelectCheckBox->setChecked(!ui->SelectCheckBox->isChecked());
    emit sigPreviewChecked(ui->SelectCheckBox->isChecked());
}

void PreviewWidget::setPreviewWidgetHover()
{
    this->setStyleSheet(QLatin1String("QFrame\n"
        "{\n"
        "background-color: rgb(0, 0, 0);\n"
        "border-radius:2px;"
        "border:1px solid rgba(74,82,90,1);"
        "}"));
}

void PreviewWidget::setPreviewWidgetDefault()
{
    this->setStyleSheet(QLatin1String("QFrame\n"
        "{\n"
        "background-color: rgb(0, 0, 0);\n"
        "}"));
}