#include "CArmOperationTab.h"

CArmImageTab::CArmImageTab(QWidget *parent)
    :QWidget(parent)
    , m_pSelectSharpenGear(nullptr)
    , m_pSelectDenoiseGear(nullptr)
    , m_pSharpenButtonGroup(nullptr)
    , m_pDenoiseButtonGroup(nullptr)
    , m_bMetaRevise(false)
    , m_bNegative(false)
    , m_bRotate(false)
    , m_bLevel(false)
    , m_bVertical(false)
    , m_bReset(false)
{ 
    setupUi(this);
    if (m_pSelectSharpenGear == nullptr)
    {
        m_pSelectSharpenGear = new CArmGearSelectDialog(CArmGearSelectDialog::sharpenGear, sharpen);
    }
    if (m_pSelectDenoiseGear == nullptr)
    {
        m_pSelectDenoiseGear = new CArmGearSelectDialog(CArmGearSelectDialog::denoiseGear, denoise);
    }
    setImageWidgetStyle();
    
    linkSlot();

}

void CArmImageTab::setImageWidgetStyle()
{
    this->setStyleSheet("background-color:rgba(14,14,14,1)");
    QPixmap contrastMap(m_styleStruct.contrast);
    contrastMap = contrastMap.scaled(contrastLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    contrastLabel->setPixmap(contrastMap);

    QPixmap brightnessMap(m_styleStruct.brightness);
    brightnessMap = brightnessMap.scaled(brightnessLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    brightnessLabel->setPixmap(brightnessMap);

    QPixmap metalMap(m_styleStruct.metal);
    metalMap = metalMap.scaled(metaLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    metaLabel->setPixmap(metalMap);

    QPixmap sharpenMap(m_styleStruct.sharpen);
    sharpenMap = sharpenMap.scaled(sharpen->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    sharpen->setPixmap(sharpenMap);

    QPixmap denoiseMap(m_styleStruct.denoise);
    denoiseMap = denoiseMap.scaled(denoise->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    denoise->setPixmap(denoiseMap);

    QPixmap m(":/modelParts/Resource/modelParts/triangle.png");
    m = m.scaled(denoiseSelectGear->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    denoiseSelectGear->setPixmap(m);
    sharpenSelectGear->setPixmap(m);

    QList<QLabel*> textLabel;
    textLabel << contrastText
        << brightnessText
        << metalText;
    foreach (QLabel *label, textLabel)
    {
        label->setText(tr("自动"));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(m_styleStruct.imageLabel);
    }

    QList <QLabel*> valueLabel;
    valueLabel << contrastValue
        << brightnessValue
        << sharpenValue
        << denoiseValue;

    foreach(QLabel *label, valueLabel)
    {
        label->setText(tr("-/-"));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(m_styleStruct.labelFont);
    }

    QPixmap metaMap(m_styleStruct.metaReviseFalse);
    metaMap = metaMap.scaled(metaRevise->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    metaRevise->setPixmap(metaMap);

    QPixmap negativeMap(m_styleStruct.negativeFalse);
    negativeMap = negativeMap.scaled(negative->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    negative->setPixmap(negativeMap);

   ValueLabel->setStyleSheet(m_styleStruct.rotateFont);
   ValueLabel->setAlignment(Qt::AlignCenter);
   ValueLabel->setText(tr("0°"));
    QPixmap rotateMap(m_styleStruct.rotateFalse);
    rotateMap = rotateMap.scaled(rotate->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    rotate->setPixmap(rotateMap);
    

    QPixmap levelMap(m_styleStruct.levelFalse);
    levelMap = levelMap.scaled(level->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    level->setPixmap(levelMap);

    QPixmap verticalMap(m_styleStruct.verticalFalse);
    verticalMap = verticalMap.scaled(vertical->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    vertical->setPixmap(verticalMap);

    QPixmap resetMap(m_styleStruct.reset);
    resetMap = resetMap.scaled(reset->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    reset->setPixmap(resetMap);

    line_6->setStyleSheet(m_styleStruct.lineStyle);

    QList<QLabel*> text;
    text << reviseText
        << negativeText
        << rotateText
        << levelText
        << verticalText
        << resetText;
    foreach (QLabel *label, text)
    {
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(m_styleStruct.text);
    }
    

    brightnessSlider->setRange(-100,100);
    brightnessSlider->setPageStep(10);
    contrastSlider->setRange(-100, 100);
    contrastSlider->setPageStep(10);

    QList<QSlider*> sliderList;
    sliderList.clear();
    sliderList<< sharpenSlider
        << denoiseSlider;
    foreach(QSlider *slider, sliderList)
    {
        slider->setStyleSheet(m_styleStruct.slider);
        slider->setRange(0, 100);
        slider->setPageStep(10);
    }


    m_metalList.clear();
    m_metalList << zero
        << one
        << two
        << three;
    foreach(QLabel *label, m_metalList)
    {
        label->setStyleSheet(m_styleStruct.metalLabelFalse);
    }


    m_pDenoiseButtonGroup = new QButtonGroup(this);
    if (m_pDenoiseButtonGroup != nullptr)
    {
        m_pDenoiseButtonGroup->addButton(m_pSelectDenoiseGear->oneGear, CRAM_GEAR_ONE);
        m_pDenoiseButtonGroup->addButton(m_pSelectDenoiseGear->twoGear, CRAM_GEAR_TWO);
        m_pDenoiseButtonGroup->addButton(m_pSelectDenoiseGear->threeGear, CRAM_GEAR_THREE);
      
        connect(m_pDenoiseButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(denoiseGear_slot(int)));

    }
    m_pSharpenButtonGroup = new QButtonGroup(this);
    if (m_pSharpenButtonGroup != nullptr)
    {
        m_pSharpenButtonGroup->addButton(m_pSelectSharpenGear->oneGear, CRAM_GEAR_ONE);
        m_pSharpenButtonGroup->addButton(m_pSelectSharpenGear->twoGear, CRAM_GEAR_TWO);
        m_pSharpenButtonGroup->addButton(m_pSelectSharpenGear->threeGear, CRAM_GEAR_THREE);
        connect(m_pSharpenButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(sharpenGear_slot(int)));
    }
   

    sharpenGear->setStyleSheet(m_styleStruct.gear);
    denoiseGear->setStyleSheet(m_styleStruct.gear);
}

void CArmImageTab::linkSlot()
{
    connect(metaRevise, SIGNAL(mouseClicked()), this, SLOT(metaRevise_slot()));
    connect(negative, SIGNAL(mouseClicked()), this, SLOT(negative_slot()));
    connect(rotate, SIGNAL(mouseClicked()), this, SLOT(rotate_slot()));
    connect(ValueLabel, SIGNAL(mouseClicked()), this, SLOT(rotate_slot()));
    connect(level, SIGNAL(mouseClicked()), this, SLOT(level_slot()));
    connect(vertical, SIGNAL(mouseClicked()), this, SLOT(vertical_slot()));
    connect(reset, SIGNAL(mouseClicked()), this, SLOT(reset_slot()));

    connect(zero, SIGNAL(mouseClicked()), this, SLOT(metalZero_slot()));
    connect(one, SIGNAL(mouseClicked()), this, SLOT(metalOne_slot()));
    connect(two, SIGNAL(mouseClicked()), this, SLOT(metalTwo_slot()));
    connect(three, SIGNAL(mouseClicked()), this, SLOT(metalThree_slot()));

    connect(sharpenSelectGear, SIGNAL(mouseClicked()), this, SLOT(sharpenSelectGear_slot()));
    connect(denoiseSelectGear, SIGNAL(mouseClicked()), this, SLOT(denoiseSelectGear_slot()));
    connect(denoise, SIGNAL(mouseClicked()), this, SLOT(denoiseSelectGear_slot()));
    connect(sharpen, SIGNAL(mouseClicked()), this, SLOT(sharpenSelectGear_slot()));
    

   

    connect(m_pSelectDenoiseGear->oneGear, SIGNAL(clicked()), this, SLOT(sharpenTwoGear_slot()));

    connect(this, SIGNAL(metalGear_sig(CARM_METAL_GEAR_ENUM)), this, SLOT(setMetalGearOperation_slot(CARM_METAL_GEAR_ENUM)));
    
}

void CArmImageTab::imageOperation(CARM_IMAGE_OPERATION_ENUM imageOperation)
{
    switch (imageOperation)
    {
    case CARM_OPERATION_METAL_REVISE:
    {
        if (m_bMetaRevise == false)
        {
            QPixmap metaMap(m_styleStruct.metaReviseTrue);
            metaMap = metaMap.scaled(metaRevise->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            metaRevise->setPixmap(metaMap);
            m_bMetaRevise = true;
            emit metaRevise_sig();
        }
        else
        {
            QPixmap metaMap(m_styleStruct.metaReviseFalse);
            metaMap = metaMap.scaled(metaRevise->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            metaRevise->setPixmap(metaMap);
            m_bMetaRevise = false;
        }
        
        QPixmap negativeMap(m_styleStruct.negativeFalse);
        negativeMap = negativeMap.scaled(negative->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        negative->setPixmap(negativeMap);

        QPixmap rotateMap(m_styleStruct.rotateFalse);
        rotateMap = rotateMap.scaled(rotate->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        rotate->setPixmap(rotateMap);

        QPixmap levelMap(m_styleStruct.levelFalse);
        levelMap = levelMap.scaled(level->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        level->setPixmap(levelMap);

        QPixmap verticalMap(m_styleStruct.verticalFalse);
        verticalMap = verticalMap.scaled(vertical->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        vertical->setPixmap(verticalMap);

        QPixmap metaReviseMap(m_styleStruct.reset);
        metaReviseMap = metaReviseMap.scaled(reset->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        reset->setPixmap(metaReviseMap);

        m_bNegative = false;
        m_bRotate = false;
        m_bLevel = false;
        m_bVertical = false;
        m_bReset = false;
    }
        break;
    case CARM_OPERATION_NEGATIVE:
    {
        QPixmap metaMap(m_styleStruct.metaReviseFalse);
        metaMap = metaMap.scaled(metaRevise->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        metaRevise->setPixmap(metaMap);

        if (m_bNegative == false)
        {
            QPixmap negativeMap(m_styleStruct.negativeTrue);
            negativeMap = negativeMap.scaled(negative->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            negative->setPixmap(negativeMap);
            m_bNegative = true;
            emit negative_sig();
        }
        else
        {
            QPixmap negativeMap(m_styleStruct.negativeFalse);
            negativeMap = negativeMap.scaled(negative->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            negative->setPixmap(negativeMap);
            m_bNegative = false;
        }
       

        QPixmap rotateMap(m_styleStruct.rotateFalse);
        rotateMap = rotateMap.scaled(rotate->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        rotate->setPixmap(rotateMap);

        QPixmap levelMap(m_styleStruct.levelFalse);
        levelMap = levelMap.scaled(level->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        level->setPixmap(levelMap);

        QPixmap verticalMap(m_styleStruct.verticalFalse);
        verticalMap = verticalMap.scaled(vertical->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        vertical->setPixmap(verticalMap);

        QPixmap metaReviseMap(m_styleStruct.reset);
        metaReviseMap = metaReviseMap.scaled(reset->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        reset->setPixmap(metaReviseMap);

        m_bMetaRevise = false;
        m_bRotate = false;
        m_bLevel = false;
        m_bVertical = false;
        m_bReset = false;
    }
        break;
    case CARM_OPERATION_ROTATE:
    {
        QPixmap metaMap(m_styleStruct.metaReviseFalse);
        metaMap = metaMap.scaled(metaRevise->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        metaRevise->setPixmap(metaMap);

        QPixmap negativeMap(m_styleStruct.negativeFalse);
        negativeMap = negativeMap.scaled(negative->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        negative->setPixmap(negativeMap);

        if (m_bRotate == false)
        {
            QPixmap rotateMap(m_styleStruct.rotateTrue);
            rotateMap = rotateMap.scaled(rotate->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            rotate->setPixmap(rotateMap);
            m_bRotate = true;
            emit rotate_sig(m_bRotate);
        }
        else
        {
            QPixmap rotateMap(m_styleStruct.rotateFalse);
            rotateMap = rotateMap.scaled(rotate->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            rotate->setPixmap(rotateMap);
            m_bRotate = false;
            emit rotate_sig(m_bRotate);
        }
       

        QPixmap levelMap(m_styleStruct.levelFalse);
        levelMap = levelMap.scaled(level->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        level->setPixmap(levelMap);

        QPixmap verticalMap(m_styleStruct.verticalFalse);
        verticalMap = verticalMap.scaled(vertical->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        vertical->setPixmap(verticalMap);

        QPixmap metaReviseMap(m_styleStruct.reset);
        metaReviseMap = metaReviseMap.scaled(reset->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        reset->setPixmap(metaReviseMap);

        m_bMetaRevise = false;
        m_bNegative = false;
        m_bLevel = false;
        m_bVertical = false;
        m_bReset = false;
    }
        break;
    case CARM_OPERATION_LEVEL:
    {
        QPixmap metaMap(m_styleStruct.metaReviseFalse);
        metaMap = metaMap.scaled(metaRevise->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        metaRevise->setPixmap(metaMap);

        QPixmap negativeMap(m_styleStruct.negativeFalse);
        negativeMap = negativeMap.scaled(negative->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        negative->setPixmap(negativeMap);

        QPixmap rotateMap(m_styleStruct.rotateFalse);
        rotateMap = rotateMap.scaled(rotate->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        rotate->setPixmap(rotateMap);

        if (m_bLevel == false)
        {
            QPixmap levelMap(m_styleStruct.levelTrue);
            levelMap = levelMap.scaled(level->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            level->setPixmap(levelMap);
            m_bLevel = true;
            emit level_sig();
        }
        else
        {
            QPixmap levelMap(m_styleStruct.levelFalse);
            levelMap = levelMap.scaled(level->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            level->setPixmap(levelMap);
            m_bLevel = false;
        }
        

        QPixmap verticalMap(m_styleStruct.verticalFalse);
        verticalMap = verticalMap.scaled(vertical->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        vertical->setPixmap(verticalMap);

        QPixmap metaReviseMap(m_styleStruct.reset);
        metaReviseMap = metaReviseMap.scaled(reset->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        reset->setPixmap(metaReviseMap);

        m_bMetaRevise = false;
        m_bNegative = false;
        m_bRotate = false;
        m_bVertical = false;
        m_bReset = false;
    }
        break;
    case CARM_OPERATION_VERTICAL:
    {
        QPixmap metaMap(m_styleStruct.metaReviseFalse);
        metaMap = metaMap.scaled(metaRevise->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        metaRevise->setPixmap(metaMap);

        QPixmap negativeMap(m_styleStruct.negativeFalse);
        negativeMap = negativeMap.scaled(negative->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        negative->setPixmap(negativeMap);

        QPixmap rotateMap(m_styleStruct.rotateFalse);
        rotateMap = rotateMap.scaled(rotate->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        rotate->setPixmap(rotateMap);

        QPixmap levelMap(m_styleStruct.levelFalse);
        levelMap = levelMap.scaled(level->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        level->setPixmap(levelMap);

        if (m_bVertical == false)
        {
            QPixmap verticalMap(m_styleStruct.verticalTrue);
            verticalMap = verticalMap.scaled(vertical->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            vertical->setPixmap(verticalMap);
            m_bVertical = true;
            emit vertical_sig();
        }
        else
        {
            QPixmap verticalMap(m_styleStruct.verticalFalse);
            verticalMap = verticalMap.scaled(vertical->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            vertical->setPixmap(verticalMap);
            m_bVertical = false;
        }
       
        QPixmap metaReviseMap(m_styleStruct.reset);
        metaReviseMap = metaReviseMap.scaled(reset->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        reset->setPixmap(metaReviseMap);
        m_bMetaRevise = false;
        m_bNegative = false;
        m_bRotate = false;
        m_bLevel = false;
        m_bReset = false;
    }
        break;
    case CARM_OPERATION_RESET:
    {
        QPixmap metaMap(m_styleStruct.metaReviseFalse);
        metaMap = metaMap.scaled(metaRevise->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        metaRevise->setPixmap(metaMap);

        QPixmap negativeMap(m_styleStruct.negativeFalse);
        negativeMap = negativeMap.scaled(negative->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        negative->setPixmap(negativeMap);

        QPixmap rotateMap(m_styleStruct.rotateFalse);
        rotateMap = rotateMap.scaled(rotate->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        rotate->setPixmap(rotateMap);

        QPixmap levelMap(m_styleStruct.levelFalse);
        levelMap = levelMap.scaled(level->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        level->setPixmap(levelMap);

        QPixmap verticalMap(m_styleStruct.verticalFalse);
        verticalMap = verticalMap.scaled(vertical->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        vertical->setPixmap(verticalMap);
        m_bMetaRevise = false;
        m_bNegative = false;
        m_bRotate = false;
        m_bLevel = false;
        m_bVertical = false;
        m_bReset = false;
    }
        break;
    default:
        break;
    }
}

void CArmImageTab::metaRevise_slot()
{
    metalText->hide();
    imageOperation(CARM_OPERATION_METAL_REVISE);
    
}
void CArmImageTab::negative_slot()
{
    imageOperation(CARM_OPERATION_NEGATIVE);
    
}
void CArmImageTab::rotate_slot()
{
    imageOperation(CARM_OPERATION_ROTATE);
    
}
void CArmImageTab::level_slot()
{
    imageOperation(CARM_OPERATION_LEVEL);
    
}
void CArmImageTab::vertical_slot()
{
    imageOperation(CARM_OPERATION_VERTICAL);
    
}
void CArmImageTab::reset_slot()
{
   
    ResetImageDialog *resetDialog = new ResetImageDialog(tr("是否将图像重置到初始状态?"));

    if (resetDialog != nullptr)
    {
        resetDialog->show();
  
        connect(resetDialog, SIGNAL(okResetImage()), this, SLOT(okResetImage_slot()));
    } 
}

void CArmImageTab::okResetImage_slot()
{
    imageOperation(CARM_OPERATION_RESET);
    emit reset_sig();
}


void CArmImageTab::metalZero_slot()
{
    metalGearOperation(METAL_ZERO_GEAR);
}

void CArmImageTab::metalOne_slot()
{
    metalGearOperation(METAL_ONE_GEAR);
}

void CArmImageTab::metalTwo_slot()
{
    metalGearOperation(METAL_TWO_GEAR);
}

void CArmImageTab::metalThree_slot()
{
    metalGearOperation(METAL_THREE_GEAR);
}

void CArmImageTab::sharpenSelectGear_slot()
{
    if (m_pSelectSharpenGear != NULL)
    {
        QRect relayButn = sharpen->geometry();
        QPoint globalPoint = widget_6->mapToGlobal(relayButn.topLeft());
        m_pSelectSharpenGear->move(globalPoint.x(), globalPoint.y() + sharpen->height());
        m_pSelectSharpenGear->show();
    }
}

void CArmImageTab::denoiseSelectGear_slot()
{
    if (m_pSelectDenoiseGear != NULL)
    {
        QRect relayButn = denoise->geometry();
        QPoint globalPoint = widget_6->mapToGlobal(relayButn.topLeft());
        m_pSelectDenoiseGear->move(globalPoint.x(), globalPoint.y() + denoise->height());
        m_pSelectDenoiseGear->show();
    }
}

void CArmImageTab::sharpenGear_slot(int gearID)
{
    switch (gearID)
    {
    case CRAM_GEAR_ONE:
        sharpenGear->setText(QString("%1").arg(1));
        emit sharpenGear_sig(CRAM_GEAR_ONE);
        break;
    case CRAM_GEAR_TWO:
        sharpenGear->setText(QString("%1").arg(2));
        emit sharpenGear_sig(CRAM_GEAR_TWO);
        break;
    case CRAM_GEAR_THREE:
        sharpenGear->setText(QString("%1").arg(3));
        emit sharpenGear_sig(CRAM_GEAR_THREE);
        break;
    default:
        break;
    }
    m_pSelectSharpenGear->hide();
}

void CArmImageTab::denoiseGear_slot(int gearID)
{
    switch (gearID)
    {
    case CRAM_GEAR_ONE:
        denoiseGear->setText(QString("%1").arg("1"));
        emit denoiseGear_sig(CRAM_GEAR_ONE);
        break;
    case CRAM_GEAR_TWO:
        denoiseGear->setText(QString("%1").arg("2"));
        emit denoiseGear_sig(CRAM_GEAR_TWO);
        break;
    case CRAM_GEAR_THREE:
        denoiseGear->setText(QString("%1").arg("3"));
        emit denoiseGear_sig(CRAM_GEAR_THREE);
        break;
        
    default:
        break;
    }
   m_pSelectDenoiseGear->hide();
}

void CArmImageTab::setMetalGearOperation_slot(CARM_METAL_GEAR_ENUM gear)
{
    switch (gear)
    {
    case METAL_ZERO_GEAR:
        for (int index = 0, num = m_metalList.size(); index < num; index++)
        {
            if (index == 0)
            {
                m_metalList.at(index)->setStyleSheet(m_styleStruct.metalLabelTrue);
            }
            else
            {
                m_metalList.at(index)->setStyleSheet(m_styleStruct.metalLabelFalse);
            }
        }
        break;
    case METAL_ONE_GEAR:
        for (int index = 0, num = m_metalList.size(); index < num; index++)
        {
            if (index == 1)
            {
                m_metalList.at(index)->setStyleSheet(m_styleStruct.metalLabelTrue);
            }
            else
            {
                m_metalList.at(index)->setStyleSheet(m_styleStruct.metalLabelFalse);
            }
        }
        break;
    case METAL_TWO_GEAR:
        for (int index = 0, num = m_metalList.size(); index < num; index++)
        {
            if (index == 2)
            {
                m_metalList.at(index)->setStyleSheet(m_styleStruct.metalLabelTrue);
            }
            else
            {
                m_metalList.at(index)->setStyleSheet(m_styleStruct.metalLabelFalse);
            }
        }
        break;
    case METAL_THREE_GEAR:
        for (int index = 0, num = m_metalList.size(); index < num; index++)
        {
            if (index == 3)
            {
                m_metalList.at(index)->setStyleSheet(m_styleStruct.metalLabelTrue);
            }
            else
            {
                m_metalList.at(index)->setStyleSheet(m_styleStruct.metalLabelFalse);
            }
        }
        break;
    default:
        break;
    }
}

void CArmImageTab::metalGearOperation(CARM_METAL_GEAR_ENUM gear)
{
   
    emit metalGear_sig(gear);
}

void CArmImageTab::setWidgetInit(ImageParam imageInit)
{
    contrastSlider->setValue(imageInit.m_contrast);
    brightnessSlider->setValue(imageInit.m_brightness);
    sharpenSlider->setValue(imageInit.m_sharpen);
    denoiseSlider->setValue(imageInit.m_denoise);

}