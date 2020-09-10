#pragma once

#include <QWidget>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QTimer>

#include "SeqFile.h"
#include "UEG_CMD.h"

class CArmWidget : public QWidget
{
    Q_OBJECT
private:
    CArmWidget();

public:
    class DetectorInfo;
    ~CArmWidget();
    // 获取此类型的一个实例
    static CArmWidget* instance();
    const DetectorInfo* GetDetectorInfo() const;
    const SeqFileBase* GetSelectedSeq() const;
    bool Init();
    void Reset();
    float FOVLaserRemainingTime() const;
    float CrossLaserRemainingTime() const;
    void CrossLaserOn();
    void CrossLaserOff();
    void FovLaserOn();
    void FovLaserOff();
    void AllLaserOn();
    void AllLaserOff();

signals:
    void SendMsg(Msg_t::Type type, const uint8Vec& arg);
    void ResetDevSig();

private:
    // 初始化raw data选择控件
    bool InitSeqSelector();

    /**
     * @brief 探测器选择控件
     * @return QWidget* 执行选择控件的指针
     */
    bool InitDetectorSelector() const;

    bool IsCrossLaserOn() { return m_crossLaserTimer.isActive(); }
    void CrossLaserOffDelay();
    bool IsFovLaserOn(){return m_fovLaserTimer.isActive();}
    void FovLaserOffDelay();
    void RasterOn();
    void RasterOff();

public slots:
    void newInfoReady(const QString& info) const;

private:
    QComboBox*      m_seqSelector;              //!< seq选择器
    QComboBox*      m_detectorSelector;         //!< 探测器选择器
    QPlainTextEdit*      m_displayWidget;   // 用于显示实时信息的控件
    QTimer  m_fovLaserTimer;
    QTimer  m_crossLaserTimer;
    bool m_rasterStatus;
};

struct CArmWidget::DetectorInfo { std::string vendor, NickName, model; };
Q_DECLARE_METATYPE(CArmWidget::DetectorInfo*)