#include "MessageTipDialog.h"



#include <qt\QtWidgets\QVBoxLayout>
#include <qt\QtWidgets\QHBoxLayout>
#include <qt\QtGui\QMouseEvent>
#include <Windows.h>

#pragma execution_character_set("utf-8")


/*************************************************************
* class   MessageTipDialog   消息提示窗
**************************************************************/
MessageTipDialog::MessageTipDialog()
    : QDialog()
    , mousePressed(false)
    , CloseBtnClicked(false)
{
    initUI();

    initLogic();
}

MessageTipDialog::~MessageTipDialog()
{

}

void MessageTipDialog::initUI()
{
    message_ui.setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}

void MessageTipDialog::initLogic()
{
    connect(message_ui.close_btn, SIGNAL(clicked()), this, SLOT(closeDialog()));

    connect(message_ui.cancelBtn, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    connect(message_ui.submitBtn, SIGNAL(clicked()), this, SLOT(submitPressed()));

}







void MessageTipDialog::mousePressEvent(QMouseEvent * e)
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

void MessageTipDialog::mouseMoveEvent(QMouseEvent * e)
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

void MessageTipDialog::mouseReleaseEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton)
    {
        mousePressed = false;
    }
    e->ignore();

}

void MessageTipDialog::cancelPressed()
{
    this->reject();
}

void MessageTipDialog::submitPressed()
{
    this->accept();
}


void MessageTipDialog::closeDialog()
{
    CloseBtnClicked = true;
    this->close();
}