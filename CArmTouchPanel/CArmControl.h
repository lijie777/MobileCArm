#ifndef CArmControl_H
#define CArmControl_H
/**
* @file CArmLabel.h
* @brief 小C界面控件
* @author 路旺明
*/
#include <QLabel>
#include <QMouseEvent>
#include <QDialog>
#include <QPixmap>
#include <QLayout>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include "CArmData.h"
#include "ui_CArmDialog.h"
#include "ui_CArmResetImageDialog.h"
#include "ui_CArmGearSelectDialog.h"
#include "ui_CArmRotateDialog.h"
#pragma execution_character_set("utf-8")

class ResetImageDialog;
class CArmGearSelectDialog;

/**
* @brief 触屏界面用的按钮
* 触屏没有按钮点击下陷状态，封装QLabel当按钮使用
*/
class CArmLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CArmLabel(QWidget *parent = Q_NULLPTR);
    ~CArmLabel();

    bool       m_bLabelClicked;

protected:
    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void mouseClicked();
    void mosueRelease();

};


/**
* @brief 曝光和仿G型臂的弹窗
* 开始曝光和仿G型臂时提示窗口
*/
class CArmDialog : public QDialog , public Ui::CArmDialog
{
public:
    explicit CArmDialog(QWidget *parent = Q_NULLPTR);

    void setImage(QString imagePth, QString text);

    void setWidgetModal(bool state);


};


/**
* @brief 触屏传输提示窗口
* 点击传输按钮，根据传输成功是否进行弹窗提示
*/
class CArmTransferDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CArmTransferDialog(bool state, QString text, QWidget *parent = Q_NULLPTR);
    ~CArmTransferDialog();
private slots:
    void closeDialog_slot();

private:
    QTimer*     m_pTimer;
};


/**
* @brief 触屏重置图像提示窗口
* 点击重置图像按钮，提示是否需要进行重置
*/
class ResetImageDialog : public QDialog, public Ui::resetImageDialog
{
    Q_OBJECT
public:
    explicit ResetImageDialog(QString text, QWidget *parent = Q_NULLPTR);

    
private slots:
    void closeDialog_slot();
    void okDialog_slot();

signals:

    void cancelResetImage();
    void okResetImage();
};

/**
* @brief 触屏锐度，降噪等级选择窗口
* 对锐度，降噪的等级选择窗口
*/
class CArmGearSelectDialog : public QFrame, public Ui::CArmSelectGear
{
    Q_OBJECT
public:
    enum CarmGearEnum
    {
        sharpenGear = 0,
        denoiseGear
    };
    explicit CArmGearSelectDialog(CarmGearEnum gear, QWidget *parent = Q_NULLPTR);
    ~CArmGearSelectDialog();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

/**
* @brief 旋转角度选择窗口
* 进行旋转操作的时候，进行旋转角度的选择
*/
class CArmRotateDialog :public QFrame, public Ui::CArmRotate
{
    Q_OBJECT
public:
    explicit CArmRotateDialog(QWidget *parent = Q_NULLPTR);
    ~CArmRotateDialog();


private slots:
    void rotateAngleChanged_slot(int value);

signals:
    void sendCurrentAngle_sig(int);

private:

    CARM_STYLE_STRUCT   m_styleStruct;
};


#endif // ! 

