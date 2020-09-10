#include "CArmVideoAdjustWidget.h"

CArmVideoAdjustWidget::CArmVideoAdjustWidget(QWidget *parent)
    : QWidget(parent)
    , m_bPlayOrPause(false)
{
    setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowOpacity(0.5);
    setAttribute(Qt::WA_TranslucentBackground);
    videoSlider->setStyleSheet("QSlider::groove:horizontal{height: 4px;background-color:rgba(66,80,96,1) ;border: 1px solid #566576;border-radius: 0px;padding - left:-1px;padding right:-1px;}"
      "QSlider::handle:horizontal\
    {\
    width: 22px;\
        background-color:rgba(255,255,255,1);\
margin-top: -10px;\
margin-bottom: -40px;\
    }"
        "QSlider::tickPosition{}");

    currenTime->setStyleSheet("background:transparent; font:16px Microsoft YaHei; color:rgba(255,255,255,1);");
    widget->setAttribute(Qt::WA_TranslucentBackground);
    widget->setWindowOpacity(0.5);

    adjustWidget->setAttribute(Qt::WA_TranslucentBackground);
    adjustWidget->setWindowOpacity(0.5);

    retreat->setStyleSheet("background:transparent;");
    playAndDisplay->setStyleSheet("background:transparent;");
    advance->setStyleSheet("background:transparent;");

    QPixmap  advanceMap(":/CArmTouchPanel/Resource/advance.png");
    advanceMap.scaled(advance->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    advance->setPixmap(advanceMap);

    QPixmap retreatMap(":/CArmTouchPanel/Resource/retreat.png");
    retreatMap.scaled(retreat->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    retreat->setPixmap(retreatMap);

    QPixmap playAndDisplayMap(":/CArmTouchPanel/Resource/startPlay.png");
    playAndDisplayMap.scaled(playAndDisplay->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    playAndDisplay->setPixmap(playAndDisplayMap);

    connect(playAndDisplay, SIGNAL(mouseClicked()), this, SLOT(playOrPauseVideo_slot()));
}

CArmVideoAdjustWidget::~CArmVideoAdjustWidget()
{

}

void CArmVideoAdjustWidget::playOrPauseVideo_slot()
{
    if (m_bPlayOrPause == false)
    {
        QPixmap playAndDisplayMap(":/CArmTouchPanel/Resource/startPlay.png");
        playAndDisplayMap.scaled(playAndDisplay->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        playAndDisplay->setPixmap(playAndDisplayMap);
        m_bPlayOrPause = true;
    }
    else
    {
        QPixmap playAndDisplayMap(":/CArmTouchPanel/Resource/donePlay.png");
        playAndDisplayMap.scaled(playAndDisplay->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        playAndDisplay->setPixmap(playAndDisplayMap);
        m_bPlayOrPause = false;
    }
    emit playOrPauseVideo(m_bPlayOrPause);
}