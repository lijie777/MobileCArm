#include "MessageDialog.h"



#include <qt\QtWidgets\QVBoxLayout>
#include <qt\QtWidgets\QHBoxLayout>
#include <qt\QtGui\QMouseEvent>
#include <Windows.h>

#pragma execution_character_set("utf-8")


/*************************************************************
* class   MessageDialog   消息提示窗
**************************************************************/
MessageDialog::MessageDialog()
    : QDialog()
    , mousePressed(false)
    , CloseBtnClicked(false)
{
    initUI();

    initLogic();
}

MessageDialog::~MessageDialog()
{

}

void MessageDialog::initUI()
{
    message_ui.setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}

void MessageDialog::initLogic()
{
    connect(message_ui.close_btn, SIGNAL(clicked()), this, SLOT(closeDialog()));

    connect(message_ui.submitBtn, SIGNAL(clicked()), this, SLOT(submitPressed()));

}







void MessageDialog::mousePressEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton &&
        message_ui.titlebar->rect().contains(e->pos()))
    {
        m_MousePressPos = e->globalPos();
        mousePressed = true;
        this->raise();
    }
    e->ignore();

}

void MessageDialog::mouseMoveEvent(QMouseEvent * e)
{
    if (mousePressed)
    {
        m_MouseMovePos = e->globalPos();
        int height = GetSystemMetrics(SM_CYFULLSCREEN);
        if (m_MouseMovePos.y() >= height) return;
        this->move(this->pos() + m_MouseMovePos - m_MousePressPos);
        m_MousePressPos = m_MouseMovePos;
        return;
    }
    e->ignore();
}

void MessageDialog::mouseReleaseEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton)
    {
        mousePressed = false;
    }
    e->ignore();

}

void MessageDialog::cancelPressed()
{
    this->reject();
}

void MessageDialog::submitPressed()
{
    this->accept();
}


void MessageDialog::closeDialog()
{
    CloseBtnClicked = true;
    this->close();
}