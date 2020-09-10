#include "CustomQLabel.h"
CustomQLabel::CustomQLabel(QWidget *parent, Qt::WindowFlags f) :
    QLabel(parent, f)
{
}
CustomQLabel::CustomQLabel(const QString &text, QWidget *parent, Qt::WindowFlags f) :
    QLabel(text, parent, f)
{
}
void CustomQLabel::mouseReleaseEvent(QMouseEvent * ev)
{
    Q_UNUSED(ev)
        emit clicked();
}