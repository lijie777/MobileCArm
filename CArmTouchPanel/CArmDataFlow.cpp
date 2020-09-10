#include "CArmDataFlow.h"

/**********************************************************/
/***************************初始值************************/
/********************************************************/
CArmInitData* CArmInitData::m_initData = nullptr;
CArmInitData::CArmInitData()
{

}

CArmInitData* CArmInitData::getInstance()
{
    if (m_initData == nullptr)
    {
        m_initData = new CArmInitData();
    }
    return  m_initData;
}

void CArmInitData::destroy()
{
    if (m_initData != nullptr)
    {
        delete m_initData;
        m_initData = nullptr;
    }
}

void CArmInitData::setSystemDefaultData(CArmConfig *configData)
{
    m_pCArmConfig = configData;


    if (sendScanData)
    {
        sendScanData();
    }
}


/**********************************************************/
/************************仿G臂测试************************/
/********************************************************/
GArmCheck* GArmCheck::m_gArmCheck = nullptr;
GArmCheck::GArmCheck()
{

}

GArmCheck* GArmCheck::getInstance()
{
    if (m_gArmCheck == nullptr)
    {
        m_gArmCheck = new GArmCheck;
    }
    return m_gArmCheck;
}

void GArmCheck::destroy()
{
    if (m_gArmCheck != nullptr)
    {
        delete m_gArmCheck;
        m_gArmCheck = nullptr;
     }
}

void GArmCheck::setPatientInfo()
{
    if (patienInfo)
    {
        patienInfo();
    }
}

void GArmCheck::setCheckData()
{
    if (needCheckData)
    {
        needCheckData();
    }
}

void GArmCheck::setCheckResult(bool state)
{
    if (checkResult)
    {
        checkResult(state);
    }
}


/**********************************************************/
/*******************触屏界面数据发生改变******************/
/********************************************************/
CArmDataChanged* CArmDataChanged::m_CArmData = nullptr;
CArmDataChanged::CArmDataChanged()
{

}

CArmDataChanged* CArmDataChanged::getInstance()
{
    if (m_CArmData == nullptr)
    {
        m_CArmData = new CArmDataChanged();
    }

    return m_CArmData;
}

void CArmDataChanged::destroy()
{

}

void CArmDataChanged::setCArmData()
{

}

void CArmDataChanged::setCArmButtonInfo()
{

}

/*将曝光数据实时的发给界面*/
void CArmDataChanged::setImageData(NewImgReceived imageData)
{
    if (loadImageData)
    {
        loadImageData(imageData);
    }
}

void CArmDataChanged::setImageDisposeData()
{

}

void CArmDataChanged::setLaserLampStatus(bool status)
{
    if (laserLampStatus)
    {
        laserLampStatus(status);
    }
}

void CArmDataChanged::setScanStatus(bool status)
{
    if (scanStatus)
    {
        scanStatus(status);
   }
}

void CArmDataChanged::setScanDataChanged()
{

}


/**********************************************************/
/**************************影像传输***********************/
/********************************************************/
CArmTransferSignal* CArmTransferSignal::m_transferObj = nullptr;
CArmTransferSignal::CArmTransferSignal()
{

}

CArmTransferSignal* CArmTransferSignal::getInstance()
{
    if (m_transferObj == nullptr)
    {
        m_transferObj = new CArmTransferSignal();
    }

    return m_transferObj;
}

void CArmTransferSignal::destroy()
{
    if (m_transferObj != nullptr)
    {
        delete m_transferObj;
        m_transferObj = nullptr;
    }
}

void CArmTransferSignal::setTransferState(bool state)
{
    if (transferPress)
    {
        transferPress(state);
    }
}

void CArmTransferSignal::setTransferResult(bool state)
{
    if (transferResult)
    {
        transferResult(state);
    }
}