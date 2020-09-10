#ifndef CARM_SC_CMD_DEF_H
#define CARM_SC_CMD_DEF_H

#pragma warning(disable:4996)

/**
* @file CArmScCmdDef.h
* @brief 定义移动C型臂设备上下位机通信消息类
* @details
* @author 张静波
*/

#include <time.h>
#include <iomanip>
#include <mutex>
#include <random>
#include <sstream>
#include <cassert>
#include <Windows.h>

#include "CArmEnum.h"
#include "Config.h"
#include "IPC_CMD_Def_CArm.h"

/// 虚函数表指针大小
#define SIZE_VTABLE_PTR sizeof(char*)

#define SC_RESP_TMO 1000        ///< 下位机响应命令超时时间(ms)
#define SC_CMD_MARK_CARM (0xAA) ///< 命令起始标识符

#pragma region 通信命令定义

//发到下位机
#define SC_CARM_GET_STATUS 0x0000       ///< 获取下位机工作状态
#define SC_CARM_DEV_RESET 0x0001        ///< 设备复位
#define SC_CARM_WARN_RESET 0x0002       ///< 报警复位
#define SC_CARM_LASER_SWITCH 0x0003     ///< 激光灯开关
#define SC_CARM_SET_KV 0x0004           ///< 设置管电压
#define SC_CARM_SET_MA 0x0005           ///< 设置管电流
#define SC_CARM_SET_FPS 0x0006          ///< 设置采集帧率
#define SC_CARM_SET_EXPOSURE 0x0007     ///< 设置单帧曝光时间
#define SC_CARM_SET_FRAME_NUM 0x0008    ///< 设置采集帧数
#define SC_CARM_SET_HOR_COLL 0x0009     ///< 设置水平方向限束器位置
#define SC_CARM_SET_VER_COLL 0x000A     ///< 设置垂直方向限束器位置
#define SC_CARM_MOVE_LIFT_COL 0x000B    ///< 升降柱连续运动的启停控制
#define SC_CARM_MOVE_X_AXIS 0x000C      ///< x轴连续运动的启停控制
#define SC_CARM_MOVE_C_ARM 0x000D       ///< C型臂连续运动的启停控制
#define SC_CARM_GET_POS_LIFT_COL 0x000E ///< 获取升降柱位置
#define SC_CARM_GET_POS_X_AXIS 0x000F   ///< 获取前后轴（x轴）位置
#define SC_CARM_GET_POS_C_ARM 0x0010    ///< 获取C型臂位置
#define SC_CARM_SET_POS_C_ARM 0x0011    ///< 控制C型臂位置
#define SC_CARM_SET_POS_X_AXIS 0x0012   ///< 控制前后轴（x轴）运动到指定位置
#define SC_CARM_SET_POS_LIFT_COL 0x0013 ///< 控制升降柱运动到指定位置
#define SC_CARM_SET_MAN_C_ARM 0x0014    ///< 使能C型臂人工转动
#define SC_CARM_SET_MAN_LIFT_COL 0x0015 ///< 使能升降柱运动人工升降
#define SC_CARM_SET_MAN_X_AXIS 0x0016   ///< 使能前后轴（x轴）人工移动
#define SC_CARM_GET_DAP_VALUE 0x0017    ///< 获取DAP当前读数
#define SC_CARM_SET_3D_TRACK_DATA 0x0018///< 设置3D扫描运动轨迹
#define SC_CARM_START_SCAN_2D 0x0019    ///< 开始采集2D图像
#define SC_CARM_START_SCAN_3D 0x001A    ///< 开始采集3D图像
#define SC_CARM_FETCH_3D_TRACK 0x0022   ///< 获取上次3D扫描记录的实际轨迹
#define SC_CARM_ENABLE_DAP 0x0023       ///< 启用/禁用DAP
#define SC_CARM_GET_KV 0x002F           ///< 查询当前管电压
#define SC_CARM_GET_MA 0x0030           ///< 查询当前管电流
#define SC_CARM_GET_CROSS_LINE_LSR_STS 0x0031   ///< 查询十字线激光灯开关状态
#define SC_CARM_GET_VIEW_RANGE_LSR_STS 0x0032   ///< 查询视野范围激光灯开关状态
#define SC_CARM_GET_MB_TEMP 0x0033              ///< 查询设备主板温度
#define SC_CARM_ENABLE_TUBE_CTRL 0x0035         ///< 通知下位机暂停/恢复球管控制
#define SC_CARM_GET_BULB_TUBE_TEMP 0x0036       ///< 查询球管温度
#define SC_CARM_RADIATION_PERMISSION 0x0037     ///< 是否允许下位机放射线
#define SC_CARM_GET_BATTERY_PERCENT 0x0038      ///< 查询下位机电池电量
#define SC_CARM_GET_RASTER_STATUS 0x0039        ///< 查询光栅的当前开关状态
#define SC_CARM_CANCEL_SCAN 0x003B      ///< 上位机通知下位机取消扫描
#define SC_CARM_SET_COLL_H_CONTINUE_MOVE 0x003C  ///< 设置水平限束器连续运动
#define SC_CARM_SET_COLL_V_CONTINUE_MOVE 0x003D  ///< 设置垂直限束器连续运动
#define SC_CARM_GET_COLL_H_POS 0x003E           ///< 获取当前水平限束器位置
#define SC_CARM_GET_COLL_V_POS 0x003F           ///< 获取当前垂直限束器位置

//从下位机发来
#define SC_CARM_EXP_BTN1_PRESSED 0x001B     ///< 曝光手闸状态变化（一级）
#define SC_CARM_EXP_BTN2_PRESSED 0x001C     ///< 曝光手闸状态变化（二级）
#define SC_CARM_PEDAL_PRESSED 0x001D        ///< 脚踏板状态变化
#define SC_CARM_LIFT_STS_CHANGED 0x001E     ///< 升降柱运动状态变化（开始运动、停止运动）
#define SC_CARM_XAXIS_STS_CHANGED 0x001F    ///< 前后轴(x轴)运动状态变化（开始运动、停止运动）
#define SC_CARM_CARM_STS_CHANGED 0x0020     ///< C型臂运动状态变化（开始运动、停止运动）
#define SC_CARM_XRAY_READY 0x0021           ///< X射线开始发射（此后采集到的影像才是实际需要的）

#define SC_CARM_WARN_REPORT 0x0024          ///< 设备报警
#define SC_CARM_COL_IN_PLACE_HOR 0x0025     ///< 水平方向限束器运动到位后下位机通知当前限束器位置
#define SC_CARM_COL_IN_PLACE_VER 0x0026     ///< 垂直方向限束器运动到位后下位机通知当前限束器位置
#define SC_CARM_MANU_AUTO_CHGD_CARM 0x0027  ///< C型臂自动/手动状态变化的通知消息
#define SC_CARM_MANU_AUTO_CHGD_Z 0x0028     ///< 升降柱自动/手动状态变化的通知消息
#define SC_CARM_MANU_AUTO_CHGD_X 0x0029     ///< 前后轴自动/手动状态变化的通知消息
#define SC_CARM_LSR_STS_CHANGED_CROSS 0x002A ///< 十字线激光灯开关状态变化
#define SC_CARM_LSR_STS_CHANGED_VIEW 0x002B ///< 视野范围激光灯开关状态变化
#define SC_CARM_CURR_POS_CARM 0x002C        ///< C型臂发生位移后的新位置通知
#define SC_CARM_CURR_POS_Z 0x002D           ///< 升降柱发生位移后的新位置通知
#define SC_CARM_CURR_POS_X 0x002E           ///< 前后轴发生位移后的新位置通知
#define SC_CARM_DEV_RESET_RES 0x0034        ///< 设备复位完成后下位机通知上位机复位结果
#define SC_CARM_RASTER_STATUS_CHANGED 0x003A    ///< 通知上位机光栅开关变化


//以下命令非下位机使用，起始类型为0x1000
#define WS_CARM_NEW_IMG_RECEIVED    0x1000      ///< 有了一帧新的影像数据
#define WS_CARM_TP_WS_NET_STATUS    0x1001      ///< 工作站和触摸屏网络连接情况
#define WS_CARM_NET_HB              0x1002      ///< 网络连接心跳
#define WS_CARM_C_2_S_HANDSHAKE     0x1003      ///< 网络连接后客户端发给服务器的握手信息
#define WS_CARM_S_2_C_ACK           0x1004      ///< 服务器收到握手信息后反馈的确认信息
#define WS_CARM_NEW_PATIENT_INFO    0x1005      ///< 更新患者信息
#define WS_CARM_IMG_TRANSFER        0x1006      ///< 图像传输
#define WS_CARM_IMG_ADJUST_PARAM    0x1007      ///< 设置影像显示参数
#define WS_CARM_SET_SCAN_PARAM      0x1008      ///< 设置拍片参数
#define WS_CARM_IPC_DEVICE_VALUE         0x1009      ///< 设备当前的值
#define WS_CARM_IPC_RESP_f          0x100a      ///< 查询结果
#define WS_CARM_IPC_DEV_STATUS      0x100b      ///< 设备状态发生变化
#define WS_CARM_IPC_SLAVE_SCAN_CTRL 0x100c      ///< 下位机扫描控制命令
#define WS_CARM_GET_X_AXIS_POS      0x100d      ///< x轴的位置
#define WS_CARM_GET_X_AXIS_POS_RESP 0x100e      ///< x轴的位置的响应
#define WS_CARM_GET_Y_AXIS_POS      0x100f      ///< y轴的位置
#define WS_CARM_GET_Y_AXIS_POS_RESP 0x1010      ///< y轴的位置的响应
#define WS_CARM_GET_Z_AXIS_POS      0x1011      ///< z轴的位置
#define WS_CARM_GET_Z_AXIS_POS_RESP 0x1012      ///< z轴的位置的响应
#define WS_CARM_GET_BULB_TUBE_TEMP  0x1013      ///< 获取球管温度
#define WS_CARM_GET_BULB_TUBE_TEMP_RESP  0x1014      ///< 获取球管温度的响应
#define WS_CARM_GET_BATTERY_PERCENT 0x1015      ///< 获取电池电量
#define WS_CARM_GET_BATTERY_PERCENT_RESP 0x1016      ///< 获取电池电量的响应
#define WS_CARM_GET_RASTER_STATUS   0x1017      ///< 获取当前光栅的状态
#define WS_CARM_GET_RASTER_STATUS_RESP   0x1018      ///< 获取当前光栅的状态的响应
#define WS_CARM_LOGIN_STATUS_CHANGED 0x1019     ///< 登录状态发生变化
#define WS_CARM_SCAN_FINISHED       0x101A      ///< 扫描结束
#define WS_CARM_UPDATE_CONFIG_INFO  0x101B      ///< 配置信息更新
#define WS_CARM_GET_ALL_DEV_INFO    0x101C      ///< 获取所有的设备信息
#define WS_CARM_IPC_DEVICE_MOVE_CTRL 0x101D     ///< 控制设备运动命令
#define WS_CARM_IPC_WS_STATUS        0x101E     ///< 工作站状态状态变化通知消息

#pragma endregion

#pragma pack(push)
#pragma pack(1)//所有与下位机交互的类的字节对齐为1字节

template <typename T>
void VecAppend(std::vector<char>& vec, const T& val)
{
    static_assert(std::is_pod<T>::value, "This function applies only to POD types");

    const auto p = (const char*)(&val);
    vec.insert(std::end(vec), p, p + sizeof(val));
}

struct IPCPatientInfo
{
    char patientID[256];        //!< 患者id
    char studyID[256];          //!< study id
    char firstName[256];        //!< 名字，utf-8编码
    char lastName[256];         //!< 姓，utf-8编码
    int age;                    //!< 患者年龄
    CArmGenderDef gender;       //!< 患者性别
    ScanPart scanPart;          //!< 扫描部位
};

struct IPCImgIdTransfer
{
    char patientID[256];        //!< 患者id
    char studyID[256];          //!< studyID
    char seriesID[256];         //!< 序列ID
    char imgID[256];            //!< 图像ID
};

// struct IPCScanParam
// {
//     CArmLastScanConfig scanfig; //!< 拍片参数
//     //bool isAuto;                //!< 是否为自动
//     //int frameCount;             //!< 本次拍片帧数
// };

template <class T, std::size_t N>
void VecAppend(std::vector<char>& vec, const T(&val)[N])
{
    static_assert(std::is_pod<T>::value, "This function applies only to POD types");

    vec.insert(vec.end(), std::begin(val), std::end(val));
}

template <typename T>
std::string ToString(const T& val)
{
    static_assert(std::is_pod<T>::value, "This function applies only to POD types");

    std::stringstream str;

    const BYTE *pStartPos = ((BYTE*)(&val));
    for (int i = 0; i < sizeof(T); ++i)
    {
        str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
    }

    return str.str();
}

static std::string ToString(const CArmLastScanConfig& val)
{
    std::stringstream str;

    str << "[kv:" << val.m_scanParam.m_kVp << "], [mA:" << val.m_scanParam.m_mA << "], [fps:" << val.m_scanParam.m_fps << "], [exposuretime:" << val.m_scanParam.m_exposureTime << "], [framecount:" << val.m_scanParam.m_frameCount << "]";

    return str.str();
}

static std::string ToString(const IPCPatientInfo& val)
{
    std::stringstream str;

    str << "[firstName:" << val.firstName << "], [lastName:" << val.lastName << "], [age:" << val.age << "]";

    return str.str();
}

static std::string ToString(const IPCImgIdTransfer& val)
{
    std::stringstream str;

    str << "[imgID:" << val.imgID << "]";

    return str.str();
}

/**
* @brief 通信基类
*/
class CArmCmdBase
{
public:
    BYTE  m_u8CmdMark;   //!< 消息头
    mutable std::uint32_t m_u16MsgLen;   //!< 包括消息头在内的整个消息的长度
    WORD m_u16MsgID;    //!< 消息ID，唯一标识一条消息的ID，由发送方动态生成，响应方以此ID作为消息ID返回。
    WORD m_u16Cmd;      //!< 消息类型，接收方根据消息类型做相应处理

    virtual ~CArmCmdBase()
    {

    }

    inline WORD Type() const
    {
        return m_u16Cmd;
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const = 0
    {
        return std::uint32_t(sizeof(*this) - SIZE_VTABLE_PTR);
    }

    /**
    * @brief 获取整个消息体的起始地址
    * @return 消息体起始地址
    */
    const BYTE* GetData() { return (const BYTE*)this + SIZE_VTABLE_PTR; };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString() = 0
    {
        std::stringstream str;

        str << std::hex << std::setw(2) << std::setfill('0') << int(m_u8CmdMark) << ' ';
        str << std::hex << std::setw(8) << std::setfill('0') << int(m_u16MsgLen) << ' ';
        str << std::hex << std::setw(4) << std::setfill('0') << int(m_u16MsgID) << ' ';
        str << std::hex << std::setw(4) << std::setfill('0') << int(m_u16Cmd);

        return str.str();
    }

    virtual void ToVec(std::vector<char>& vec) const = 0
    {
        ::VecAppend(vec, m_u8CmdMark);
        ::VecAppend(vec, m_u16MsgLen);
        ::VecAppend(vec, m_u16MsgID);
        ::VecAppend(vec, m_u16Cmd);
    }

    /**
    * @brief 计算一个新的消息ID
    * @return 消息ID
    */
    WORD GetNewMsgID()
    {
        static std::random_device rd;
        static std::uniform_int_distribution<int> dist(0, 0xFFFF);

        return dist(rd);
    };
};

static void AdjustCmdLenField(const CArmCmdBase* msg)
{
    msg->m_u16MsgLen = msg->GetSize();
}

static std::vector<char> ToVec(const CArmCmdBase* msg)
{
#if 0
    assert(msg->m_u16MsgLen == msg->GetSize());
#endif

    AdjustCmdLenField(msg);

    std::vector<char> vec;
    msg->ToVec(vec);
    return vec;
}

/**
* @brief 从下位机收到的通用消息类
*/
class CArmRawCmd : public CArmCmdBase
{
public:

    /**
    * @brief 默认构造函数
    */
    CArmRawCmd()
    {
        m_u8CmdMark = 0;
        m_u16MsgLen = 0;
        m_u16MsgID = 0;
        m_u16Cmd = 0;

        m_nDataLen = 0;
        m_pu8Data = nullptr;
    };

    /**
     * @brief 拷贝构造函数
     * @param e
     * @return 
     */
    CArmRawCmd(const CArmRawCmd &e)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = e.m_u16MsgLen;
        m_u16MsgID = e.m_u16MsgID;
        m_u16Cmd = e.m_u16Cmd;

        m_nDataLen = e.m_nDataLen;
        m_pu8Data = new BYTE[m_nDataLen];
        std::memcpy(m_pu8Data, e.m_pu8Data, m_nDataLen);
    }

    CArmRawCmd& operator=(const CArmRawCmd& e)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = e.m_u16MsgLen;
        m_u16MsgID = e.m_u16MsgID;
        m_u16Cmd = e.m_u16Cmd;

        delete[] m_pu8Data;
        m_nDataLen = e.m_nDataLen;
        m_pu8Data = new BYTE[m_nDataLen];
        std::memcpy(m_pu8Data, e.m_pu8Data, m_nDataLen);

        return *this;
    }

    /**
     * @brief 构造函数
     * @param u16MsgLen 消息长度
     * @param u16MsgID 消息ID
     * @param u16Cmd 消息
     * @param pu8Data 消息的数据
     * @param nDataLen pu8Data的长度
     * @return 
     */
    CArmRawCmd(std::uint32_t u16MsgLen, WORD u16MsgID, WORD u16Cmd, BYTE *pu8Data, int nDataLen)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = u16MsgLen;
        m_u16MsgID = u16MsgID;
        m_u16Cmd = u16Cmd;

        m_nDataLen = nDataLen;
        m_pu8Data = new BYTE[m_nDataLen];
        std::memcpy(m_pu8Data, pu8Data, m_nDataLen);
    };

    ~CArmRawCmd()
    {
        delete [] m_pu8Data;
    }

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        vec.insert(vec.end(), (char*)m_pu8Data, (((char*)m_pu8Data) + m_nDataLen));
    }

    BYTE *m_pu8Data;//!< 消息数据
    int m_nDataLen;//!< 消息数据部分的长度

    /**
    * @brief 获取整个消息体的起始地址
    * @return 消息体起始地址
    */
    virtual const BYTE* GetData() { return (const BYTE*)this + SIZE_VTABLE_PTR; }

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (int i = 0; i < sizeof(CArmCmdBase) - SIZE_VTABLE_PTR; i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        for (int i = 0; i < m_nDataLen; i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)m_pu8Data[i] << ' ';
        }

        return str.str();
    };

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return CArmCmdBase::GetSize() + m_nDataLen;
    };
};

class CArmUpdateConfigInfo : public CArmRawCmd
{
public:
    using CArmRawCmd::CArmRawCmd;

    CArmUpdateConfigInfo(BYTE *pu8Data, int nDataLen)
        :CArmRawCmd(0, GetNewMsgID(), WS_CARM_UPDATE_CONFIG_INFO, pu8Data, nDataLen)
    {
        m_u16MsgLen = GetSize();
    }
};

/**
* @brief 无参数的消息类
*/
class CArmCmdNoParam : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdNoParam() = delete;

    CArmCmdNoParam(WORD u16Cmd)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();
        m_u16Cmd = u16Cmd;
    }

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 激光灯开关消息类
*/
class CArmCmdLaserSwitch : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdLaserSwitch() = delete;

    CArmCmdLaserSwitch(OptionLaserSwitch opt, bool bOn)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();
        m_u16Cmd = SC_CARM_LASER_SWITCH;

        m_u8Option = (BYTE)opt;
        m_u8On = bOn ? 1 : 0;
    }

    /// 激光灯选项，0-全部激光灯，1-十字线激光灯，2-视野范围激光灯
    BYTE m_u8Option;

    /// 打开或关闭，0-关闭，1-打开
    BYTE m_u8On;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Option);
        VecAppend(vec, m_u8On);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 设置管电压消息类
*/
class CArmCmdSetKV : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetKV() = delete;

    CArmCmdSetKV(float fKV)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();
        m_u16Cmd = SC_CARM_SET_KV;

        m_fKV = fKV;
    }

    /// 管电压，kV
    float m_fKV;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_fKV);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 设置管电流消息类
*/
class CArmCmdSetMA : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetMA() = delete;

    CArmCmdSetMA(float fMA)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();
        m_u16Cmd = SC_CARM_SET_MA;

        m_fMA = fMA;
    }

    /// 管电流，mA
    float m_fMA;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_fMA);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 设置采集帧率消息类
*/
class CArmCmdSetFPS : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetFPS() = delete;

    CArmCmdSetFPS(BYTE u8FPS)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();
        m_u16Cmd = SC_CARM_SET_FPS;

        m_u8FPS = u8FPS;
    }

    /// 采集帧率
    BYTE m_u8FPS;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8FPS);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 设置单帧曝光消息类
*/
class CArmCmdSetExposure : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetExposure() = delete;

    CArmCmdSetExposure(BYTE u8Exp)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();
        m_u16Cmd = SC_CARM_SET_EXPOSURE;

        m_u8Exp = u8Exp;
    }

    /// 单帧曝光时间，单位ms
    BYTE m_u8Exp;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Exp);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 设置采集帧数消息类
*/
class CArmCmdSetFrameNum : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetFrameNum() = delete;

    CArmCmdSetFrameNum(WORD u16FramNum)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();
        m_u16Cmd = SC_CARM_SET_FRAME_NUM;

        m_u16FramNum = u16FramNum;
    }

    /// 采集帧数
    WORD m_u16FramNum;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u16FramNum);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 限束器位置控制消息类
*/
class CArmCmdSetCollimator : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetCollimator() = delete;

    /**
     * @brief 构造函数
     * @param opt 限束器方向
     * @param fPos 相对于原点的距离，单位mm
     * @return 
     */
    CArmCmdSetCollimator(OptionCollimator opt, float speed, float fPos)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = (opt == OptionCollimator::Hor) ? SC_CARM_SET_HOR_COLL : SC_CARM_SET_VER_COLL;

        m_fSpeed = speed;
        m_fPos = fPos;
    }

    /// 前后轴移动速度，单位mm/s
    float m_fSpeed;
    /// 限束器位置，单位mm
    float m_fPos;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_fSpeed);
        VecAppend(vec, m_fPos);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 升降柱连续运动停止控制消息类
*/
class CArmCmdMoveLiftCol : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdMoveLiftCol() = delete;

    /**
    * @brief 构造函数
    * @param opt 运动方向
    * @param fSpeed 运动速度，单位mm/s
    * @return
    */
    CArmCmdMoveLiftCol(OptionLiftColMove opt, float fSpeed)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_MOVE_LIFT_COL;

        m_u8Direct = (BYTE)opt;
        m_fSpeed = fSpeed;
    }

    /// 运动方向，0-停止，1-向上，2-向下
    BYTE m_u8Direct;

    /// 运动速度，单位mm/s
    float m_fSpeed;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Direct);
        VecAppend(vec, m_fSpeed);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief x轴连续运动停止控制消息类
*/
class CArmCmdMoveXAxis : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdMoveXAxis() = delete;

    /**
    * @brief 构造函数
    * @param opt 运动方向
    * @param fSpeed 运动速度，单位mm/s
    * @return
    */
    CArmCmdMoveXAxis(OptionXAxisMove opt, float fSpeed)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_MOVE_X_AXIS;

        m_u8Direct = (BYTE)opt;
        m_fSpeed = fSpeed;
    }

    /// 运动方向，0-停止，1-向前，2-向后
    BYTE m_u8Direct;

    /// 运动速度，单位mm/s
    float m_fSpeed;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Direct);
        VecAppend(vec, m_fSpeed);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief C型臂连续运动停止控制消息类
*/
class CArmCmdMoveCArm : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdMoveCArm() = delete;

    /**
    * @brief 构造函数
    * @param opt 运动方向
    * @param fSpeed 运动速度，单位rad/s
    * @return
    */
    CArmCmdMoveCArm(OptionCArmMove opt, float fSpeed)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_MOVE_C_ARM;

        m_u8Direct = (BYTE)opt;
        m_fSpeed = fSpeed;
    }

    /// 运动方向，0-停止，1-顺时针旋转，2-逆时针旋转
    BYTE m_u8Direct;

    /// 运动速度，单位rad/s
    float m_fSpeed;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Direct);
        VecAppend(vec, m_fSpeed);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 控制C型臂转动到指定角度消息类
*/
class CArmCmdSetCArmAngle : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetCArmAngle() = delete;

    /**
    * @brief 构造函数
    * @param fSpeed C型臂的运动角速度，rad/s
    * @param fAngle C型臂的角度
    * @return
    */
    CArmCmdSetCArmAngle(float fSpeed, float fAngle)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_SET_POS_C_ARM;

        m_fSpeed = fSpeed;
        m_fAngle = fAngle;
    }

    /// C型臂的运动角速度，rad/s
    float m_fSpeed;

    /// 角度
    float m_fAngle;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_fSpeed);
        VecAppend(vec, m_fAngle);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 控制前后轴(x轴)运动到指定位置消息类
*/
class CArmCmdSetXAxisPos : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetXAxisPos() = delete;

    /**
    * @brief 构造函数
    * @param fSpeed x轴移动速度
    * @param fPos x轴的位置
    * @return
    */
    CArmCmdSetXAxisPos(float fSpeed, float fPos)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_SET_POS_X_AXIS;

        m_fSpeed = fSpeed;
        m_fPos = fPos;
    }

    /// 前后轴移动速度，单位mm/s
    float m_fSpeed;

    /// x轴相对于原点的位置
    float m_fPos;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_fSpeed);
        VecAppend(vec, m_fPos);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 控制升降柱（z轴）运动到指定位置消息类
*/
class CArmCmdSetLiftColPos : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetLiftColPos() = delete;

    /**
    * @brief 构造函数
    * @param fSpeed 升降柱升降速度，单位mm/s
    * @param fPos 升降柱的位置
    * @return
    */
    CArmCmdSetLiftColPos(float fSpeed, float fPos)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_SET_POS_LIFT_COL;

        m_fSpeed = fSpeed;
        m_fPos = fPos;
    }

    /// 升降柱升降速度，单位mm/s
    float m_fSpeed;

    /// 升降柱相对于原点的位置
    float m_fPos;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_fSpeed);
        VecAppend(vec, m_fPos);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 使能C型臂人工转动消息类
*/
class CArmCmdSetManuCArm : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetManuCArm() = delete;

    /**
    * @brief 构造函数
    * @param m 自动/手动
    * @return
    */
    CArmCmdSetManuCArm(CArmAxisCtrlDef m)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_SET_MAN_C_ARM;

        m_u8Manu = (BYTE)m;
    }

    /// 使能人工对C型臂操作，0-不可以人工操作，1-可以人工操作
    BYTE m_u8Manu;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Manu);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 使能升降柱(z轴)人工升降消息类
*/
class CArmCmdSetManuLiftCol : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetManuLiftCol() = delete;

    /**
    * @brief 构造函数
    * @param m 自动/手动
    * @return
    */
    CArmCmdSetManuLiftCol(CArmAxisCtrlDef m)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_SET_MAN_LIFT_COL;

        m_u8Manu = (BYTE)m;
    }

    /// 使能人工对升降柱操作，0-不可以人工操作，1-可以人工操作
    BYTE m_u8Manu;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Manu);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 使能前后轴(x轴)人工移动消息类
*/
class CArmCmdSetManuXAxis : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSetManuXAxis() = delete;

    /**
    * @brief 构造函数
    * @param m 自动/手动
    * @return
    */
    CArmCmdSetManuXAxis(CArmAxisCtrlDef m)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_SET_MAN_X_AXIS;

        m_u8Manu = (BYTE)m;
    }

    /// 使能人工对前后轴操作，0-不可以人工操作，1-可以人工操作
    BYTE m_u8Manu;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Manu);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 发送3D轨迹数据消息类
*/
class CArmCmdSend3DTrackData : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdSend3DTrackData() = delete;

    /**
    * @brief 构造函数
    * @param u16Num 轨迹点数，每个轨迹点由三个float构成：C型臂角度、x轴位置和z轴位置
    * @param pPoints 轨迹点数据
    * @return
    */
    CArmCmdSend3DTrackData(WORD u16Num, const float *pPoints)
    {
        m_u16Num = u16Num;

        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();
        m_u16Cmd = SC_CARM_SET_3D_TRACK_DATA;

        size_t trackDataSize = m_u16Num * 3 * sizeof(float);

        m_pData = new BYTE[m_u16MsgLen];
        BYTE *pTemp = m_pData;

        *pTemp = SC_CMD_MARK_CARM;  pTemp += sizeof(m_u8CmdMark);
        memcpy(pTemp, &m_u16MsgLen, sizeof(m_u16MsgLen)); pTemp += sizeof(m_u16MsgLen);
        memcpy(pTemp, &m_u16MsgID, sizeof(m_u16MsgID)); pTemp += sizeof(m_u16MsgID);
        memcpy(pTemp, &m_u16Cmd, sizeof(m_u16Cmd)); pTemp += sizeof(m_u16Cmd);
        memcpy(pTemp, &m_u16Num, sizeof(m_u16Num)); pTemp += sizeof(m_u16Num);

        std::memcpy(pTemp, pPoints, trackDataSize);
    }

    ~CArmCmdSend3DTrackData()
    {
        if (m_pData != nullptr)
        {
            delete[] m_pData;
            m_pData = nullptr;
        }
    }

    /// 轨迹点数，每个轨迹点由三个float构成：C型臂角度、x轴位置和z轴位置
    WORD m_u16Num;

    /// 轨迹数据
    BYTE *m_pData;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u16Num);
        vec.insert(vec.end(), (char*)m_pData, ((char*)m_pData + 3 * sizeof(float) * m_u16Num));
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR - sizeof(m_pData) + m_u16Num * 3 * sizeof(float));
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };

    const BYTE* GetData()
    {
        return m_pData;
    }
};

/**
* @brief 开始2D扫描消息类
*/
class CArmCmdStartScan2D : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdStartScan2D() = delete;

    /**
    * @brief 构造函数
    * @param bXRay 扫描时是否发射X射线
    * @return
    */
    CArmCmdStartScan2D(bool bXRay)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_START_SCAN_2D;

        m_u8XRay = bXRay ? 1 : 0;
    }

    /// 是否发射x射线，0-不发射，1-发射
    BYTE m_u8XRay;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8XRay);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 开始3D扫描消息类
*/
class CArmCmdStartScan3D : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdStartScan3D() = delete;

    /**
    * @brief 构造函数
    * @param u8Direct 扫描方向，0-反向扫描，1-正向扫描
    * @param bXRay 扫描时是否发射X射线
    * @return
    */
    CArmCmdStartScan3D(OptionCArmMove dr, bool bXRay)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_START_SCAN_3D;

        m_u8Direct = (BYTE)dr;
        m_u8XRay = bXRay ? 1 : 0;
    }

    /// 扫描方向，0-反向扫描，1-正向扫描
    BYTE m_u8Direct;

    /// 是否发射x射线，0-不发射，1-发射
    BYTE m_u8XRay;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Direct);
        VecAppend(vec, m_u8XRay);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 禁用/启用DAP消息类
*/
class CArmCmdEnableDAP : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdEnableDAP() = delete;

    /**
    * @brief 构造函数
    * @param bEnable 禁用/启用DAP，true-启用，false-禁用
    * @return
    */
    CArmCmdEnableDAP(bool bEnable)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_ENABLE_DAP;

        m_u8Enable = bEnable ? 1 : 0;
    }

    /// 禁用/启用DAP，1-启用，0-禁用
    BYTE m_u8Enable;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Enable);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 通知下位机暂停/恢复球管控制消息类
*/
class CArmCmdEnableTubeCtrl : public CArmCmdBase
{
public:
    /**
    * @brief 禁用默认构造函数
    */
    CArmCmdEnableTubeCtrl() = delete;

    /**
    * @brief 构造函数
    * @param bEnable 暂停/恢复球管控制，true-恢复控制，false-暂停控制
    * @return
    */
    CArmCmdEnableTubeCtrl(bool bEnable)
    {
        m_u8CmdMark = SC_CMD_MARK_CARM;
        m_u16MsgLen = GetSize();
        m_u16MsgID = GetNewMsgID();

        m_u16Cmd = SC_CARM_ENABLE_TUBE_CTRL;

        m_u8Enable = bEnable ? 1 : 0;
    }

    /// 暂停/恢复球管控制，1-恢复控制，0-暂停控制
    BYTE m_u8Enable;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_u8Enable);
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    virtual std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /**
    * @brief 将消息转换为以16进制显示的字符串
    * @return 16进制显示的字符串
    */
    virtual std::string ToString()
    {
        std::stringstream str;

        const BYTE *pStartPos = GetData();
        for (std::size_t i = 0; i < GetSize(); i++)
        {
            str << std::hex << std::setw(2) << std::setfill('0') << (int)pStartPos[i] << ' ';
        }

        return str.str();
    };
};

/**
* @brief 脚踏板状态变化
*/
class CArmCmdPedalPressed : public CArmCmdNoParam
{
public:
    CArmCmdPedalPressed(CArmPedalDef pedal, CArmExpBtnDef level, CArmBtnStatus status)
        : CArmCmdNoParam{ SC_CARM_PEDAL_PRESSED }
    {
        m_u16MsgLen = GetSize();
        m_pedal = pedal;
        m_level = level;
        m_status = status;
    }

    /**
    * @brief 获取整个消息的长度
    * @return 整个消息的长度
    */
    std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };

    /// 脚踏板索引
    CArmPedalDef m_pedal;
    /// 脚踏板的等级
    CArmExpBtnDef m_level;
    /// 此脚踏的状态
    CArmBtnStatus m_status;

    std::string ToString() override
    {
        return CArmCmdBase::ToString() + " : "
            + (CArmPedalDef::Left == m_pedal ? "left" : "right")
            + ", "
            + (CArmExpBtnDef::Level1 == m_level? "level1":"level2")
            + ", "
            + (CArmBtnStatus::Pressed == m_status?"pressed":"released");
    };

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_pedal);
        VecAppend(vec, m_level);
        VecAppend(vec, m_status);
    }
};

template <std::uint16_t cmd>
struct NoParam : CArmCmdNoParam
{
    NoParam() : CArmCmdNoParam{ cmd } {}
};

template <std::uint16_t cmd, typename T_arg, std::size_t s_arg>
struct Param_1 : CArmCmdNoParam
{
    static_assert(sizeof(T_arg) == s_arg, "Wrong param size.");
    static_assert(std::is_pod<T_arg>::value, "This function applies only to POD types");

    Param_1(T_arg arg)
        : CArmCmdNoParam{ cmd }
        , m_arg{ arg }
    {
        m_u16MsgLen = GetSize();
    }
    T_arg m_arg;

    void ToVec(std::vector<char>& vec) const override
    {
        CArmCmdBase::ToVec(vec);
        VecAppend(vec, m_arg);
    }

    std::string ToString() override
    {
        return CArmCmdBase::ToString() + " : " + ::ToString(m_arg);
    };

    std::uint32_t GetSize()  const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR);
    };
};

/**
 * @brief x射线已经准备好
 */
using CArmCmdXRayReady = NoParam<SC_CARM_XRAY_READY>;

/**
 * @brief 通知上位机限束器位置 - 水平方向
 */
//using CArmCmdCollimatorPosH = Param_1<SC_CARM_COL_IN_PLACE_HOR, float, 4>;

/**
 * @brief 通知上位机限束器位置-垂直方向
 */
//using CArmCmdCollimatorPosV = Param_1<SC_CARM_COL_IN_PLACE_VER, float, 4>;

/**
 * @brief 通知上位机手动 / 自动控制变化 - C型臂（Y轴）
 */
using CArmCmdAMChangedY = Param_1<SC_CARM_MANU_AUTO_CHGD_CARM, CArmAxisCtrlDef, 1>;

/**
 * @brief 通知上位机手动/自动控制变化-升降柱（z轴）
 */
using CArmCmdAMChangedZ = Param_1<SC_CARM_MANU_AUTO_CHGD_Z, CArmAxisCtrlDef, 1>;

/**
 * @brief 通知上位机手动/自动控制变化-前后轴（x轴）
 */
using CArmCmdAMChangedX = Param_1<SC_CARM_MANU_AUTO_CHGD_X, CArmAxisCtrlDef, 1>;

/**
 * @brief 通知上位机激光灯开关变化-十字线激光灯
 */
using CArmCmdLaserStatusChangedCross = Param_1<SC_CARM_LSR_STS_CHANGED_CROSS, CArmOnOffDef, 1>;

/**
 * @brief 通知上位机激光灯开关变化-视野范围激光灯
 */
using CArmCmdLaserStatusChangedView = Param_1<SC_CARM_LSR_STS_CHANGED_VIEW, CArmOnOffDef, 1>;

/**
 * @brief 通知上位机C型臂（Y轴）当前角度
 */
using CArmCmdYCurPos = Param_1<SC_CARM_CURR_POS_CARM, float, 4>;

 /**
  * @brief 通知上位机升降柱（z轴）当前高度
  */
using CArmCmdZCurPos = Param_1<SC_CARM_CURR_POS_Z, float, 4>;

 /**
  * @brief 通知上位机前后轴（x轴）当前位置
  */
using CArmCmdXCurPos = Param_1<SC_CARM_CURR_POS_X, float, 4>;

/**
* @brief 通知上位机升降柱（z轴）运动状态变化
*/
using CArmCmdMotionStatusZ = Param_1<SC_CARM_LIFT_STS_CHANGED, CArmAxisMotionDef, 1>;

/**
* @brief 通知上位机前后轴（x轴）运动状态变化
*/
using CArmCmdMotionStatusX = Param_1<SC_CARM_XAXIS_STS_CHANGED, CArmAxisMotionDef, 1>;

/**
* @brief 通知上位机C型臂（Y轴）运动状态变化
*/
using CArmCmdMotionStatusY = Param_1<SC_CARM_CARM_STS_CHANGED, CArmAxisMotionDef, 1>;

/**
 * @brief 下位机一级曝光按钮状态变化
 */
using CArmCmdExpBtn1Pressed = Param_1<SC_CARM_EXP_BTN1_PRESSED, CArmBtnStatus, 1>;

/**
 * @brief 下位机二级曝光按钮状态变化
 */
using CArmCmdExpBtn2Pressed = Param_1<SC_CARM_EXP_BTN2_PRESSED, CArmBtnStatus, 1>;

/**
 * @brief 通知上位机设备复位完成
 */
using CArmCmdDevResetDone = Param_1<SC_CARM_DEV_RESET_RES, unsigned short, 2>;

/**
 * @brief 通知上位机设备报警信息
 */
using CArmCmdDevWarnReport = Param_1<SC_CARM_WARN_REPORT, unsigned short, 2>;

/**
 * @brief 当有新的影像数据产生时
 */
struct NewImgReceived : CArmCmdNoParam
{
    NewImgReceived(const char* imgId, int idx, int w, int h, unsigned short*pImageData, std::uint16_t ww, std::uint16_t wc)
        : CArmCmdNoParam{ WS_CARM_NEW_IMG_RECEIVED }
        , m_seriesId{}
        , m_imgId{}
        , m_idx{ idx }
        , m_bitsAllocated{ 16 }
        , m_bitStored{ 12 }
        , m_highBit{ 11 }
        , m_pixelIsSigned{ false }
        , m_w{ w }
        , m_h{ h }
        , m_windowWidth{ww}
        , m_windowCenter{wc}
    {
        std::strncpy(m_imgId, imgId, sizeof(m_imgId)-1);
        m_img = new std::uint16_t[m_w*m_h];
        std::memcpy(m_img, pImageData, m_w*m_h*sizeof(*m_img));
        m_u16MsgLen = GetSize();
    }

    NewImgReceived()
        : CArmCmdNoParam{ WS_CARM_NEW_IMG_RECEIVED }
        , m_seriesId{}
        , m_imgId{}
        , m_bitsAllocated{16}
        , m_bitStored{12}
        , m_highBit{11}
        , m_pixelIsSigned{false}
        , m_idx{}
        , m_w{}
        , m_h{}
        , m_windowWidth{}
        , m_windowCenter{}
        , m_img{}
    {

    }

    ~NewImgReceived()
    {
        delete [] m_img;
    }

    char m_seriesId[128];
    char m_imgId[128];
    int  m_bitsAllocated;
    int  m_bitStored;
    int  m_highBit;
    bool m_pixelIsSigned;
    const int m_idx;
    const int m_w;
    const int m_h;
    const std::uint16_t m_windowWidth;
    const std::uint16_t m_windowCenter;
    unsigned short* m_img;

    void ToVec(std::vector<char>& vec) const override
    {
        m_u16MsgLen = GetSize();

        CArmCmdBase::ToVec(vec);
        vec.insert(std::end(vec), (const char*)(&m_seriesId), (const char*)(&m_img));
        vec.insert(std::end(vec), (char*)m_img, ((char*)m_img + m_w*m_h*sizeof(short)));
    }

    std::uint32_t GetSize() const override
    {
        return (std::uint32_t)(sizeof(*this) - SIZE_VTABLE_PTR - sizeof(m_img) + m_w*m_h*sizeof(*m_img));
    };

    std::string ToString() override
    {
        return CArmCmdBase::ToString()
            + " : "+ std::to_string(m_idx)
            + ' ' + std::to_string(m_w)
            + ' ' + std::to_string(m_h)
            + ' ' + std::to_string(m_windowWidth)
            + ' ' + std::to_string(m_windowCenter);
    };
};

/**
 * @brief 触摸屏和工作站网络连接发送变化
 */
using CArmCmdTpWsNetStatus = Param_1<WS_CARM_TP_WS_NET_STATUS, CArmNetStatus, sizeof(CArmNetStatus)>;

/**
 * @brief 网络连接心跳
 */
using CArmCmdNetHB = NoParam<WS_CARM_NET_HB>;

/**
 * @brief 网络连接后客户端发给服务器的握手信息
 * 参数1: 是否为长连接
 */
using CArmCmdC2SHandShake = Param_1<WS_CARM_C_2_S_HANDSHAKE, bool, sizeof(bool)>;

// 下位机通知上位机光栅状态发生变化
using CArmCmdRasterStatusChanged = Param_1<SC_CARM_RASTER_STATUS_CHANGED, bool, sizeof(bool)>;

/**
 * @brief 服务器收到握手信息后反馈的确认信息
 */
using CArmCmdS2CACK = NoParam<WS_CARM_S_2_C_ACK>;

/**
 * @brief 更新患者信息
 * 参数1: IPCPatientInfo类型对象，其中的信息为本次最新的患者信息
 */
using CArmCmdNewPatientInfo = Param_1<WS_CARM_NEW_PATIENT_INFO, IPCPatientInfo, sizeof(IPCPatientInfo)>;

/**
 * @brief 图像传输命令
 * 参数1: IPCImgIdTransfer类型对象，其中的信息能够定位到一张确定的影像
 */
using CArmCmdImgTransfer = Param_1<WS_CARM_IMG_TRANSFER, IPCImgIdTransfer, sizeof(IPCImgIdTransfer)>;

/**
 * @brief 设置图像显示参数
 * 参数1: ImageParam类型对象，其中的信息描述一个图像各种调节参数
 */
using CArmCmdImgAdjustParam = Param_1<WS_CARM_IMG_ADJUST_PARAM, ImageParam, sizeof(ImageParam)>;

/**
 * @brief 设置图像显示参数
 * 参数1: ImageParam类型对象，其中的信息描述一个图像各种调节参数
 */
using CArmCmdScanParam = Param_1<WS_CARM_SET_SCAN_PARAM, CArmLastScanConfig, sizeof(CArmLastScanConfig)>;

struct CollContinueMoveData
{
    //OptionCollimator coll;
    OptionCollimatorMoveDirection direction;
    float speed;
};

using CArmCmdCollContinueMoveH = Param_1<SC_CARM_SET_COLL_H_CONTINUE_MOVE, CollContinueMoveData, sizeof(CollContinueMoveData)>;
using CArmCmdCollContinueMoveV = Param_1<SC_CARM_SET_COLL_V_CONTINUE_MOVE, CollContinueMoveData, sizeof(CollContinueMoveData)>;

/**
 * @brief 查询球管温度
 * 参数1: float，当前球管温度
 */
//using CArmCmdBulbTubeTemp = NoParam<SC_CARM_GET_BULB_TUBE_TEMP>;
//using CArmCmdBulbTubeTempResp = Param_1<SC_CARM_GET_BULB_TUBE_TEMP, float, sizeof(float)>;

/**
* @brief 是否允许下位机放射线
* 参数1: CArmPermissionDef，是否允许
*/
using CArmCmdRadiationPermission = Param_1<SC_CARM_RADIATION_PERMISSION, CArmPermissionDef, 1>;

/**
* @brief 查询下位机电池电量
* 参数1: float，当前下位机电池电量百分比
*/
//using CArmCmdBatteryPercent = NoParam<SC_CARM_GET_BATTERY_PERCENT>;

struct DeviceValue_
{
    CArmDeviceValueTypeDef ValueType() const
    {
        return m_type;
    }

    template <CArmDeviceValueTypeDef type_>
    auto Get() const
    {
        return GetValueRef<type_>();
    }

    template <CArmDeviceValueTypeDef type_, typename T>
    void Set(const T val)
    {
        GetValueRef<type_>() = val;
    }

    template <CArmDeviceValueTypeDef type_, std::enable_if_t<
        (CArmDeviceValueTypeDef::CURRENT_COLL_H_POS == type_)
        || (CArmDeviceValueTypeDef::CURRENT_COLL_V_POS == type_)
        || (CArmDeviceValueTypeDef::CURRENT_X_AXIS_POS == type_)
        || (CArmDeviceValueTypeDef::CURRENT_Y_AXIS_POS == type_)
        || (CArmDeviceValueTypeDef::CURRENT_Z_AXIS_POS == type_)
        || (CArmDeviceValueTypeDef::CURRENT_BATTERY_PERCENT == type_)
        || (CArmDeviceValueTypeDef::CURRENT_BULB_TUBE_TEMP == type_),int> = 0>
    float& GetValueRef() const
    {
        m_type = type_;
        return floatVal;
    }

    template <CArmDeviceValueTypeDef type_, std::enable_if_t<
        (CArmDeviceValueTypeDef::CURRENT_RASTER_STATUS == type_),int> = 0>
    int& GetValueRef() const
    {
        m_type = type_;
        return intVal;
    }

    template <CArmDeviceValueTypeDef type_, std::enable_if_t<
        (CArmDeviceValueTypeDef::CURRENT_DAP_VALUE == type_), int> = 0>
    DAPValue& GetValueRef() const
    {
        m_type = type_;
        return dap;
    }

private:
    mutable CArmDeviceValueTypeDef m_type;
    union
    {
        mutable float floatVal;
        mutable int intVal;
        mutable DAPValue dap;
    };
};
// 设备相关的数值更新
using CArmCmdDeviceValue = Param_1<WS_CARM_IPC_DEVICE_VALUE, DeviceValue_, sizeof(DeviceValue_)>;

struct DeviceMove_
{
    CArmDeviceMoveTypeDef m_type;
    float velocity;
    float pos;
    int direction;
};

using CArmCmdDeviceMoveCmd = Param_1<WS_CARM_IPC_DEVICE_MOVE_CTRL, DeviceMove_, sizeof(DeviceMove_)>;

// 设备状态发生变化
using CArmCmdDevStatusChanged = Param_1<WS_CARM_IPC_DEV_STATUS, ErrorCode, sizeof(ErrorCode)>;

// 工作站状态变化通知消息
using CArmCmdWorkStationStatus = Param_1<WS_CARM_IPC_WS_STATUS, CArmWorkStationStatusTypeDef, sizeof(CArmWorkStationStatusTypeDef)>;

// 下位机扫描控制命令
using CArmCmdSlaveScanCtrl = Param_1<WS_CARM_IPC_SLAVE_SCAN_CTRL, SlaveScanCtrlType, sizeof(SlaveScanCtrlType)>;

// 登录状态发生变化，true表示已登录，false表示未登录
using CArmCmdLoginStatusChanged = Param_1<WS_CARM_LOGIN_STATUS_CHANGED, bool, sizeof(bool)>;

// 扫描结束
using CArmCmdScanFinished = NoParam<WS_CARM_SCAN_FINISHED>;

// 获取所有的设备信息
using CArmCmdGetAllDevInfo = NoParam<WS_CARM_GET_ALL_DEV_INFO>;

#pragma pack(pop)//恢复字节对齐设置

#endif //CARM_SC_CMD_DEF_H
