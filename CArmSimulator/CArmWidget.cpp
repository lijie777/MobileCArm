#include <fstream>
#include <chrono>

#include <QPushButton>
#include <QVBoxLayout>
#include <QString>
#include <QVariant>
#include <QGroupBox>
#include <QLabel>
#include <QTimer>

#include "common.h"
#include "CArmWidget.h"
#include "SeqFileParser.h"
#include "SystemConfigHandler.h"

using namespace std::literals::chrono_literals;

static const auto laser_on_interval{ 10000ms };

CArmWidget::CArmWidget()
    : m_seqSelector(new QComboBox(this))
    , m_detectorSelector(new QComboBox(this))
    , m_displayWidget(new QPlainTextEdit)
    , m_rasterStatus{}
{
    auto vMain = new QVBoxLayout;

    {
        struct Item { const Msg_t::Type type;  const uint8Vec arg; };
        auto getWidget = [this](const QString& hint, const std::vector<Item>& msg)
        {
            auto btn = new QPushButton(hint);
            connect(btn, &QPushButton::clicked, [this, msg]
            {
                for (const auto& i : msg)
                {
                    emit SendMsg(i.type, i.arg);
                }
            });
            return btn;
        };
        auto getWidget2 = [this](const QString& hint, std::function<void()> func)
        {
            auto btn = new QPushButton(hint);
            connect(btn, &QPushButton::clicked, [func] { func(); });
            return btn;
        };
        auto getLayout = [](const std::vector<QWidget*>& w)
        {
            auto layout = new QHBoxLayout;
            for (auto i : w)
            {
                layout->addWidget(i);
            }
            return layout;
        };
        auto getLabel = [](const QString& hint)
        {
            auto label = new QLabel(hint);
            label->setFixedWidth(150);
            return label;
        };
        auto msgLayout = new QVBoxLayout;
        msgLayout->setSpacing(10);
        msgLayout->addLayout(getLayout({getLabel("一级曝光手闸"),
            getWidget("按下", {{Msg_t::FIRST_EXPOSURE_BTN_STATUS_, { 1 }}}),
            getWidget("松开", {{Msg_t::FIRST_EXPOSURE_BTN_STATUS_, { 0 }}}) }));
        msgLayout->addLayout(getLayout({ getLabel("二级曝光手闸"),
            getWidget("按下", { {Msg_t::SECOND_EXPOSURE_BTN_STATUS_, { 1 }} }),
            getWidget("松开", { {Msg_t::SECOND_EXPOSURE_BTN_STATUS_, { 0 }} }) }));

        msgLayout->addLayout(getLayout({getLabel("左脚一级踏板"),
            getWidget("按下",{ { Msg_t::PEDAL_STATUS_,{ 0, 0, 1 } } }),
            getWidget("松开",{ { Msg_t::PEDAL_STATUS_,{ 0, 0, 0 } } }) }));
        msgLayout->addLayout(getLayout({ getLabel("左脚二级踏板"),
            getWidget("按下",{ { Msg_t::PEDAL_STATUS_,{ 0, 1, 1 } } }),
            getWidget("松开",{ { Msg_t::PEDAL_STATUS_,{ 0, 1, 0 } } }) }));
        msgLayout->addLayout(getLayout({ getLabel("右脚一级踏板"),
            getWidget("按下",{ { Msg_t::PEDAL_STATUS_,{ 1, 0, 1 } } }),
            getWidget("松开",{ { Msg_t::PEDAL_STATUS_,{ 1, 0, 0 } } }) }));
        msgLayout->addLayout(getLayout({ getLabel("右脚二级踏板"),
            getWidget("按下",{ { Msg_t::PEDAL_STATUS_,{ 1, 1, 1 } } }),
            getWidget("松开",{ { Msg_t::PEDAL_STATUS_,{ 1, 1, 0 } } }) }));
#if 0
        msgLayout->addLayout(getLayout({ getWidget("通知上位机脚踏板状态变化:按下", { {Msg_t::PEDAL_STATUS_, { 1 }} }),
            getWidget("通知上位机脚踏板状态变化:松开", { {Msg_t::PEDAL_STATUS_, { 0 }} }) }));
#endif

#if 0
        msgLayout->addLayout(getLayout(getWidget("通知上位机升降柱（z 轴）运动状态变化:停止", Msg_t::Z_MOTION_STATUS_, { 1 }),
            getWidget("通知上位机升降柱（z 轴）运动状态变化:开始", Msg_t::Z_MOTION_STATUS_, { 0 })));
        msgLayout->addLayout(getLayout(getWidget("通知上位机升降柱（x 轴）运动状态变化:停止", Msg_t::X_MOTION_STATUS_, { 1 }),
            getWidget("通知上位机升降柱（x 轴）运动状态变化:开始", Msg_t::X_MOTION_STATUS_, { 0 })));
        msgLayout->addLayout(getLayout(getWidget("通知上位机升降柱（y 轴）运动状态变化:停止", Msg_t::Y_MOTION_STATUS_, { 1 }),
            getWidget("通知上位机升降柱（y 轴）运动状态变化:开始", Msg_t::Y_MOTION_STATUS_, { 0 })));
#endif

#if 0
        msgLayout->addLayout(getLayout(getWidget("通知上位机 X 射线开始发射", Msg_t::X_RAY_EMIT_, {}),
            getWidget("通知上位机设备报警信息", Msg_t::ALARM_MSG_, { 0,0 })));
#endif

#if 0
        msgLayout->addLayout(getLayout(getWidget("通知上位机限束器位置-水平方向", Msg_t::COLLIMATOR_POS_H_, { 0,0,0,0 }),
            getWidget("通知上位机限束器位置-垂直方向", Msg_t::COLLIMATOR_POS_V_, { 0,0,0,0 })));
#endif

        msgLayout->addLayout(getLayout({getLabel("控制变化-C 型臂（Y 轴）"),
            getWidget("手动", { {Msg_t::Y_HAND_CTRL_STATUS_, { 1 }} }),
            getWidget("自动", { {Msg_t::Y_HAND_CTRL_STATUS_, { 0 }} }) }));
        msgLayout->addLayout(getLayout({ getLabel("控制变化-升降柱（z 轴）"),
            getWidget("手动", { {Msg_t::Z_HAND_CTRL_STATUS_, { 1 }} }),
            getWidget("自动", { {Msg_t::Z_HAND_CTRL_STATUS_, { 0 }} }) }));
        msgLayout->addLayout(getLayout({ getLabel("控制变化-前后轴（x 轴）"),
            getWidget("手动", { {Msg_t::X_HAND_CTRL_STATUS_, { 1 }} }),
            getWidget("自动", { {Msg_t::X_HAND_CTRL_STATUS_, { 0 }} }) }));

        msgLayout->addLayout(getLayout({ getLabel("全部激光"),
            getWidget2("关闭", [this] { AllLaserOff(); }),
            getWidget2("开启", [this] { AllLaserOn(); }) }));
        msgLayout->addLayout(getLayout({getLabel("十字线激光灯"),
            getWidget2("关闭", [this] { CrossLaserOff(); }),
            getWidget2("开启", [this] { CrossLaserOn();  }) }));
        msgLayout->addLayout(getLayout({ getLabel("视野范围激光灯"),
            getWidget2("关闭",[this] { FovLaserOff(); }),
            getWidget2("开启",[this] { FovLaserOn(); }) }));

        msgLayout->addLayout(getLayout({ getLabel("光栅"),
            getWidget2("关闭",[this] { RasterOff(); }),
            getWidget2("开启",[this] { RasterOn(); }) }));

        msgLayout->addWidget(getWidget("设备报警信息", { { Msg_t::ALARM_MSG_,{ 0,0 } } }));
        msgLayout->addWidget(getWidget2("设备复位", [this] { emit ResetDevSig(); }));
#if 0
        msgLayout->addWidget(getWidget("通知上位机 C 型臂（Y 轴）当前角度", Msg_t::Y_ANGLE_STATUS_, { 0,0,0,0 }));
        msgLayout->addWidget(getWidget("通知上位机升降柱（z 轴）当前高度", Msg_t::Z_HEIGHT_STATUS_, { 0,0,0,0 }));
        msgLayout->addWidget(getWidget("通知上位机前后轴（x 轴）当前位置", Msg_t::X_POS_STATUS_, { 0,0,0,0 }));
#endif

        auto groupBox0 = new QGroupBox("消息按钮");
        groupBox0->setLayout(msgLayout);
        vMain->addWidget(groupBox0);
    }

    // seq文件选择
    auto seqLayout = new QVBoxLayout;
    seqLayout->addWidget(m_seqSelector);
    auto groupBox1 = new QGroupBox("SEQ文件选择");
    groupBox1->setLayout(seqLayout);
    vMain->addWidget(groupBox1);

    {
    auto groupBox = new QGroupBox("探测器选择");
    auto layout_ = new QHBoxLayout;
    layout_->addWidget(m_detectorSelector);
    groupBox->setLayout(layout_);
    vMain->addWidget(groupBox);
    }

    // 显示实时信息区域
    auto vLayout0 = new QVBoxLayout;
    vLayout0->addWidget(m_displayWidget);
    auto groupBox3 = new QGroupBox("实时信息");
    groupBox3->setLayout(vLayout0);
    vMain->addWidget(groupBox3);

    setLayout(vMain);

    // 设置激光灯定时器为单次的
    connect(&m_fovLaserTimer, &QTimer::timeout, [this] { FovLaserOff(); });
    connect(&m_crossLaserTimer, &QTimer::timeout, [this] { CrossLaserOff(); });
}

void CArmWidget::CrossLaserOn()
{
    if (IsCrossLaserOn())
    {
        return;
    }

    emit SendMsg(Msg_t::CROSS_LASER_SWITCH_STATUS_, { 1 });

    CrossLaserOffDelay();
}

void CArmWidget::CrossLaserOff()
{
    if (IsCrossLaserOn())
    {
        m_crossLaserTimer.stop();
        emit SendMsg(Msg_t::CROSS_LASER_SWITCH_STATUS_, { 0 });
    }
}

void CArmWidget::CrossLaserOffDelay()
{
    m_crossLaserTimer.start(laser_on_interval.count());
}

void CArmWidget::FovLaserOn()
{
    if (IsFovLaserOn())
    {
        return;
    }

    emit SendMsg(Msg_t::FOV_LASER_SWITCH_STATUS_, { 1 });

    FovLaserOffDelay();
}
void CArmWidget::RasterOn()
{
    if (m_rasterStatus)
    {
        return;
    }

    m_rasterStatus = true;

    emit SendMsg(Msg_t::RASTER_STATUS_CHANGED, { 1 });
}
void CArmWidget::RasterOff()
{
    if (m_rasterStatus)
    {
        m_rasterStatus = false;
        emit SendMsg(Msg_t::RASTER_STATUS_CHANGED, { 0 });
    }
}
void CArmWidget::FovLaserOff()
{
    if (IsFovLaserOn())
    {
        m_fovLaserTimer.stop();
        emit SendMsg(Msg_t::FOV_LASER_SWITCH_STATUS_, { 0 });
    }
}

void CArmWidget::AllLaserOn()
{
    CrossLaserOn(); 
    FovLaserOn(); 
    FovLaserOffDelay(); 
    CrossLaserOffDelay();
}

void CArmWidget::AllLaserOff()
{
    CrossLaserOff(); 
    FovLaserOff();
}

void CArmWidget::FovLaserOffDelay()
{
    m_fovLaserTimer.start(laser_on_interval.count());
}

CArmWidget::~CArmWidget()
{

}

// 获取此类型的一个实例
CArmWidget* CArmWidget::instance()
{
    static auto ins = new CArmWidget;
    return ins;
}

const CArmWidget::DetectorInfo* CArmWidget::GetDetectorInfo() const
{
    return m_detectorSelector->currentData().value<CArmWidget::DetectorInfo*>();
}

const SeqFileBase* CArmWidget::GetSelectedSeq() const
{
    return m_seqSelector->currentData().value<SeqFileBase*>();
}

bool CArmWidget::Init()
{
    return InitDetectorSelector() && InitSeqSelector();;
}

void CArmWidget::Reset()
{
    m_crossLaserTimer.stop();
    m_fovLaserTimer.stop();
}

float CArmWidget::FOVLaserRemainingTime() const
{
    const auto time_ = m_fovLaserTimer.remainingTime();
    if (-1 == time_)
    {
        return 0.f;
    }
    return time_ / 1000.f;
}

float CArmWidget::CrossLaserRemainingTime() const
{
    const auto time_ = m_crossLaserTimer.remainingTime();
    if (-1 == time_)
    {
        return 0.f;
    }
    return time_ / 1000.f;
}

// 初始化raw data选择控件
bool CArmWidget::InitSeqSelector()
{
    for (const auto& i : SeqFileParser::getSeqVec())
    {
        m_seqSelector->addItem(i->description().c_str(), QVariant::fromValue(i));
    }

    return true;
}

bool CArmWidget::InitDetectorSelector() const
{
    const auto cfgFile = UCommonUtility::GetExeDir() + "\\Config\\" + SYSCONFIG_FILE_NAME;
    std::ifstream ifs(cfgFile);
    cereal::XMLInputArchive archive(ifs);
    USystemConfigHandler sys;
    archive(sys);

    for (const auto& vendor : sys.GetDetectorInfoList())
    {
        if (vendor.first.empty())
        {
            continue;
        }

        for (const auto& info : vendor.second)
        {
            if (info.strNickName.empty())
            {
                continue;
            }
            const auto detector_info = new CArmWidget::DetectorInfo{vendor.first, info.strNickName, info.strModel};
            m_detectorSelector->addItem(QString(detector_info->vendor.data())+" "+detector_info->NickName.data(), QVariant::fromValue(detector_info));
        }
    }

    return true;
}

void CArmWidget::newInfoReady(const QString& info) const
{
    if (!info.isEmpty())
    {
        m_displayWidget->appendPlainText(info);
    }
}
