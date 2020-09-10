/**
 * @file PreviewWidget.h
 * @author AndyGao
 * @brief 预览小窗口组件
 */
#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include "ui_PreviewWidget.h"
#include "DbDataStructures.h"
#include <QMouseEvent>
#include <QTimer>
#include <QHBoxLayout>

#pragma execution_character_set("utf-8")

namespace Ui {
	class PreviewWidget;
}

/**
 * @brief DICOM预览自定义子控件类
 */
class PreviewWidget : public QFrame
{
	Q_OBJECT

public:
	/**
	 * @brief DICOM预览自定义子控件类构造函数
	 * @param parent 
	 * @return  
	 */
	explicit PreviewWidget(int num = 1,QFrame *parent = 0);

	/**
	 * @brief DICOM预览自定义子控件类析构函数
	 * @return  
	 */
	~PreviewWidget();

    /**
    * @brief 调用该函数，初始化功能右屏窗口布局
    */
    void initUI();

    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

	/**
	 * @brief 更新界面显示
	 * @param mTmp_Series 拍摄(包括临时表)信息（小C）
	 * @param filepath 预览图路径
	 * @return void 
	 */
	void updateDisplay(DB_TmpSeries mTmp_Series,QString filepath=NULL);

    void changePreviewNumber(int num);

signals:
    void sigPreviewChecked(bool ischecked);
    void sigFoldButtonChecked(PreviewWidget * view, bool ischecked);
    void sigPreviewDoubleClicked(std::string dicomPath);

public:
    std::string strDicomPath;    //!< DICOM路径
    QHBoxLayout *BelongHLayout;
    int PreviewNumber;

private:
	Ui::PreviewWidget *ui;        //!< DICOM预览自定义窗口UI

	QString UniqueID;            //!< 控件唯一ID

    QTimer *m_clickedTimer = nullptr;

private:
	/**
	 * @brief 设置预览图子控件鼠标悬浮风格
	 * @return void 
	 */
	void setPreviewWidgetHover();

	/**
	 * @brief 设置预览图子控件默认风格
	 * @return void 
	 */
	void setPreviewWidgetDefault();

    private slots:
    void slotSingleClicked();
    void con_FoldCheckBox_clicked();
    void con_ImageCountLabel_clicked();

protected:
	/**
	 * @brief 事件过滤器
	 * @param mObject 对象
	 * @param mEvent 时间
	 * @return bool 事件过滤器状态
	 */
	bool eventFilter(QObject *mObject, QEvent *mEvent);

    void mouseDoubleClickEvent(QMouseEvent *e);

    void mousePressEvent(QMouseEvent *e);
};

#endif // PREVIEWWIDGET_H
