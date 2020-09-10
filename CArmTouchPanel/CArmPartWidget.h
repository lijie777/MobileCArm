/**
* @file CArmPartWidget.h
* @brief 身体的具体部位封装，包括身体部位图和部位说明
* @author 李杰
*/
#pragma once

#include <QWidget>
#include <QString>
#include <QEvent>
#include <QMouseEvent>
#include "ui_CArmPartWidget.h"

class CArmPartWidget : public QWidget
{
    Q_OBJECT

public:
    CArmPartWidget(QWidget *parent = Q_NULLPTR);
    ~CArmPartWidget();

    /**
    * @brief 对当前部位的描述
    * @param string 当前部位的描述内容
    * @return void
    */
    void setDescription(QString string);
    /**
    * @brief 对当前部位设置图标
    * @param string 图标路径
    * @return void
    */
    void setPicture(char* path);

    /**
    * @brief 设置当前的图标按钮和描述标签的边框，不加的话会显示出来
    * @param void
    * @return void
    */
    void styleSet(void);

signals:
    /**
    * @brief 当选择了一个部位时候发出的信号
    * @param void
    * @return void
    */
    void bodyPartSignal();

protected:
    /**
    * @brief 当选择了一个部位时候发出的信号
    * @param Object 当前操作的对象
    * @param event 事件类型
    * @return void
    */
    bool eventFilter(QObject *Object, QEvent *event);


private:
    Ui::CArmPartWidget ui;
};
