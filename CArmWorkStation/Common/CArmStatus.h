#pragma once


#include <list>
#include "CArmCMD.h"




class CArmStatus
{
public:

    //移动C型臂状态码
    enum STATUS_CODE
    {
        SC_IDLE = 0,        //!<闲置状态
        SC_READY,       //!<准备拍片状态
        SC_EXPOSURE,       //!<执行拍片状态
        SC_ABORT,          //!<拒绝拍片状态
        SC_LOGOUT           //!<工作站退出登陆状态
    };

public:
    CArmStatus(int type);
    ~CArmStatus();

    /**
    * @brief 调用该函数，执行指令
    */
    virtual void doCmd(const CArmCMD& cmdObject) {};


protected:

    /// 状态类型
    const int _type;
};






class CArmIdleStatus : public CArmStatus
{
public:
    CArmIdleStatus() : CArmStatus{ CArmStatus::STATUS_CODE::SC_IDLE } { }
    ~CArmIdleStatus() { }

    /**
    * @brief 调用该函数，执行指令
    */
    virtual void doCmd(const CArmCMD& cmdObject) { }

};


class CArmReadyStatus : public CArmStatus
{
public:
    CArmReadyStatus() : CArmStatus{ CArmStatus::STATUS_CODE::SC_READY } { }
    ~CArmReadyStatus() { }

    /**
    * @brief 调用该函数，执行指令
    */
    virtual void doCmd(const CArmCMD& cmdObject) { }
};


class CArmExposureStatus : public CArmStatus
{
public:
    CArmExposureStatus() : CArmStatus{ CArmStatus::STATUS_CODE::SC_EXPOSURE } { }
    ~CArmExposureStatus() { }

    /**
    * @brief 调用该函数，执行指令
    */
    virtual void doCmd(const CArmCMD& cmdObject) { }
};


class CArmAbortStatus : public CArmStatus
{
public:
    CArmAbortStatus() : CArmStatus{ CArmStatus::STATUS_CODE::SC_ABORT } { }
    ~CArmAbortStatus() { }

    /**
    * @brief 调用该函数，执行指令
    */
    virtual void doCmd(const CArmCMD& cmdObject) { }
};


class CArmLogoutStatus : public CArmStatus
{
public:
    CArmLogoutStatus() : CArmStatus{ CArmStatus::STATUS_CODE::SC_LOGOUT } { }
    ~CArmLogoutStatus() { }

    /**
    * @brief 调用该函数，执行指令
    */
    virtual void doCmd(const CArmCMD& cmdObject) { }
};


