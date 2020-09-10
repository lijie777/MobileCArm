#include "CArmDose.h"
#include "CArmData.h"

CArmDose::CArmDose(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    // 隐藏标题栏
    setWindowFlags(Qt::FramelessWindowHint);
    // 置顶当前窗口
    setWindowFlags(Qt::ToolTip);
    // 设置模态对话框
 //   setAttribute(Qt::WA_ShowModal, true);

    SCANSETTING_STYLE_STRUCT scanInfo;
    // 设置边框
    setStyleSheet(scanInfo.frameBorder + scanInfo.PPSfontStyle);
}

CArmDose::~CArmDose()
{
}
