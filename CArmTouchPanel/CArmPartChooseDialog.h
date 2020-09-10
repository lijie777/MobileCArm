/**
* @file CArmPartChooseDialog.h
* @brief 身体部位选择界面，暂时包括九种部位
* @author 李杰
*/
#pragma once

#include <QDialog>

#include "ui_CArmPartChooseDialog.h"
#include "CArmPartWidget.h"

class CArmPartChooseDialog : public QDialog
{
    Q_OBJECT

public:
    CArmPartChooseDialog(QWidget *parent = Q_NULLPTR);
    ~CArmPartChooseDialog();

    CArmPartWidget*        m_pPartWidget; //!< 身体的具体部位封装，包括身体部位图和部位说明

signals:

    /// 在多种身体部位选择一个扫描部位信号
    void bodyPartSignal(CArmPartWidget*);

public slots:
    /**
    * @brief 在多种身体部位选择一个扫描部位
    * @param void
    * @return
    */
    void bodyPartSlot(void);

private:
    /**
    * @brief 初始化及槽函数的连接
    * @param void
    * @return
    */
    void initBodyPartDialog(void);

public:
    Ui::CArmPartChooseDialog ui;

    
};
