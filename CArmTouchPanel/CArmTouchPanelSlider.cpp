#include "CArmTouchPanelSlider.h"

TouchPanelSlider::TouchPanelSlider(QWidget *parent)
    : QSlider(parent)
{
    this->setStyleSheet("QSlider::groove:horizontal{height: 4px;background - color:rgba(66, 80, 96, 1);\
                border: 1px solid #566576;border - radius: 0px; padding - left:-1px;padding right : -1px;}"
        "QSlider::sub - page:horizontal{height: 4px;background: transparent;\
                border: 1px solid transparent;\
                border - radius: 2px;}"
        "QSlider::handle:horizontal{width: 22px;background - color:rgba(255,255,255,1);margin - top: -10px;\
                margin - bottom: -10px;border - radius: 5px;}");
}


void TouchPanelSlider::paintEvent(QPaintEvent *event)
{
    QSlider::paintEvent(event);

    int sliderH = 4;                    // 滑动条的高
    int blockW = 26;                    // 滑块的宽
    int h = height();                   // 滑动条的高
    int w = width();                    // 滑动条的宽
    int min = minimum();                // 滑动条最小值
    int max = maximum();                // 滑动条最大值
    int middle = (max + min) / 2;       // 滑动条中间值
    int current = value();              // 滑动条当前值

    if (current == middle) return;      // 当前点为中点时不进行绘制

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(QColor(0, 145, 255));
    QBrush lineBrush(QColor(0, 145, 255), Qt::SolidPattern);
    pen.setWidth(sliderH);
    painter.setBrush(lineBrush);
    painter.setPen(pen);

    float delta = 1.0 * blockW / 2 / (max - middle);            // 滑块偏移量计算斜率
    int sign = (current < middle) ? -1 : 1;                     // 偏移方向
    float offset = (current - middle) * delta + sign * blockW / 2;// 滑块偏移量
    int x1 = 1.0 * (current - min) / (max - min) * w - offset;  // 直线起点
    int x2 = 1.0 * (middle - min) / (max - min) * w;            // 直线终点

                                                                // 显示处理
    if (x1 == x2) return;
    if (current > middle && (x1 - x2) < 0) return;
    else if (current < middle && (x2 - x1) < 0) return;

    painter.drawLine(x1, h / 2, x2, h / 2);                     // 绘制直线
}