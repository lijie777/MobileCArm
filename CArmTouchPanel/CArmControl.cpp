#include "CArmControl.h"

/***************************************************************************/
/*************************封装Label鼠标点击事件****************************/
/*************************************************************************/
CArmLabel::CArmLabel(QWidget *parent)
    : QLabel(parent)
    , m_bLabelClicked(false)
{

}
CArmLabel::~CArmLabel()
{

}

void CArmLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit mouseClicked();
        m_bLabelClicked = true;
    }

    QLabel::mousePressEvent(event);
}

void CArmLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit mosueRelease();
    }
    QLabel::mouseReleaseEvent(event);
}

/***************************************************************************/
/**************************曝光、仿G臂提示窗口*****************************/
/*************************************************************************/
CArmDialog::CArmDialog( QWidget *parent)
    :QDialog(parent)
{
    setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    statusLabel->setStyleSheet("QLabel{background:transparent;}");
 
    this->setAttribute(Qt::WA_ShowModal, true);
  
    this->setMinimumSize(150, 150);
    this->setMaximumSize(150, 150); 
}

void CArmDialog::setImage(QString imagePth, QString text)
{
    if (text == "禁止曝光")
    {
        textLabel->setStyleSheet("QLabel{\
        background:transparent;\
        border:none;\
        color:#FC3923;\
        font: 16px Microsoft YaHei;}");
        textLabel->setText(text);
    }
    else if (text == "曝光中")
    {
        textLabel->setStyleSheet("QLabel{\
        background:transparent;\
        border:none;\
        color:#FFFFD42A;\
        font: 16px Microsoft YaHei;}");
        textLabel->setText(text);
    }
    textLabel->setAlignment(Qt::AlignCenter);
    QPixmap pixmap(imagePth);
    pixmap = pixmap.scaled(statusLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    statusLabel->setPixmap(pixmap);
}

void CArmDialog::setWidgetModal(bool state)
{
    if (state == true)
    {
        this->setAttribute(Qt::WA_ShowModal, true);
    }
    
}


/***************************************************************************/
/*******************************传输结果提示窗口***************************/
/*************************************************************************/
CArmTransferDialog::CArmTransferDialog(bool state, QString text, QWidget *parent)
    :QDialog(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setAttribute(Qt::WA_ShowModal, true);
    this->setFixedSize(328, 40);
    QLabel* pText = new QLabel(this);
    pText->setStyleSheet("QLabel{font:14px Microsoft YaHei;color:rgba(255,255,255,1)};");
    if (state == false)
    {
        this->setStyleSheet("background-color:rgba(252,57,35,1)");
        
        pText->setText(text);
    }
    else if (state == true)
    {
        this->setStyleSheet("background-color:rgba(0,145,255,1)");
        pText->setText(text);
    }
   
    pText->setAlignment(Qt::AlignHCenter);
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(pText);
    this->setLayout(layout);
    
    m_pTimer = new QTimer(this);
    /*设置单次触发*/
    m_pTimer->setSingleShot(true);
    m_pTimer->start(1000);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(closeDialog_slot()));

}

CArmTransferDialog::~CArmTransferDialog()
{
    if (m_pTimer != nullptr)
    {
        delete m_pTimer;
        m_pTimer = nullptr;
    }
}

void CArmTransferDialog::closeDialog_slot()
{
    this->close();
}



/***************************************************************************/
/*******************************重置图像提示窗口***************************/
/*************************************************************************/
ResetImageDialog::ResetImageDialog(QString text, QWidget *parent)
    :QDialog(parent)
{
    setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setAttribute(Qt::WA_ShowModal, true);
    this->setStyleSheet("background-color:rgba(14,20,23,1)");
    titleWidget->setStyleSheet("background-color:rgba(31,36,40,1)");
    titleLabel->setText(tr("重置图像"));
    titleLabel->setStyleSheet("QLabel{font:18px Microsoft YaHei; color:rgba(180,196,217,1)};");

    textLabel->setText(text);
    textLabel->setStyleSheet("QLabel{font:16px Microsoft YaHei;color:rgba(180,196,217,1)};");
    QPixmap resetImage(":/CArmTouchPanel/Resource/resetDialog.png");
    resetImage = resetImage.scaled(imagLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    imagLabel->setPixmap(resetImage);

    cancel->setStyleSheet("QPushButton{ background:qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 rgba(69,80,92,1), stop : 1.0 rgba(58,71,86,1));"
"border:0px solid rgb(69,80,92);"
   "border - radius:2px;"
        "color:rgba(255,255,255,1);"
"font: normal 16px  Microsoft YaHei;}");
    
    ok->setStyleSheet("QPushButton{ background:qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 rgba(60,140,227,1), stop : 1.0 rgba(32,108,195,1));"
"border:0px solid rgb(60,140,227);"
   "border - radius:2px;"
        "color:rgba(255,255,255,1);"
        "font: normal 16px  Microsoft YaHei;}");

    cancel->setText(tr("取消"));
    ok->setText(tr("确定"));
    connect(cancel, SIGNAL(clicked()), this, SLOT(closeDialog_slot()));
    connect(ok, SIGNAL(clicked()), this, SLOT(okDialog_slot()));
}

void ResetImageDialog::closeDialog_slot()
{
    emit cancelResetImage();
    this->close();
}

void ResetImageDialog::okDialog_slot()
{
    emit okResetImage();
    this->close();
}


/***************************************************************************/
/****************************锐化、降噪等级选择窗口************************/
/*************************************************************************/
CArmGearSelectDialog::CArmGearSelectDialog(CarmGearEnum gear, QWidget *parent)
    :QFrame(parent)
{
    setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->installEventFilter(this);
    this->setFocusPolicy(Qt::StrongFocus);

    this->setStyleSheet("QFrame{background-color:rgba(14,14,14,1); border:1px solid #4A525A;}");
    frame->setStyleSheet("QFrame{background:transparent; border:0px solid #4A525A;}");
    const QString t = "QToolButton{background:transparent;border:0px solid #303840;border-radius:0px;color:#E1E8F1;font:14px Microsoft YaHei;padding-top:2;}";
    oneGear->setStyleSheet(t);
    twoGear->setStyleSheet(t);
    threeGear->setStyleSheet(t);
    if (gear == sharpenGear)
    {
        oneGear->setText(tr("锐化一"));
        twoGear->setText(tr("锐化二"));
        threeGear->setText(tr("锐化三"));
    }
    else
    {
        oneGear->setText(tr("降噪一"));
        twoGear->setText(tr("降噪二"));
        threeGear->setText(tr("降噪三"));
    }
    
}

CArmGearSelectDialog::~CArmGearSelectDialog()
{

}

bool CArmGearSelectDialog::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseButtonPress: 
    case QEvent::WindowDeactivate:
        this->close();
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}


/***************************************************************************/
/****************************图像旋转，角度调节窗口************************/
/*************************************************************************/
CArmRotateDialog::CArmRotateDialog(QWidget *parent)
    : QFrame(parent)
{
    setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    rotateValue->setStyleSheet("background-color:rgba(0,0,0,0.4); font:21px Microsoft YaHei; color:rgba(255,255,255,1);");
    rotateValue->setAlignment(Qt::AlignCenter);
    setWindowOpacity(0.5);
    setAttribute(Qt::WA_TranslucentBackground);

    rotateValue->setText(QString("%1°").arg("0"));

    connect(rotateSlider, SIGNAL(valueChanged(int)), this, SLOT(rotateAngleChanged_slot(int)));
  
    rotateSlider->setRange(-180, 180);

    this->hide();

}

CArmRotateDialog::~CArmRotateDialog()
{

}

void CArmRotateDialog::rotateAngleChanged_slot(int value)
{
    rotateValue->setText(QString("%1°").arg(value));
    emit sendCurrentAngle_sig(value);
}

