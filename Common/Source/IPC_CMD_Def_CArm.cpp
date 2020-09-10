#include <IPC_CMD_Def_CArm.h>

const std::string CArmIPCCMD::UPDATE_DEV_STATUS("UpdateDevStatus");

const std::string CArmIPCCMD::GET_SC_STATUS("GetScStatus");
const std::string CArmIPCCMD::GET_SC_STATUS_RESP("GetScStatusResp");

const std::string CArmIPCCMD::DEV_RESET("DeviceReset");
const std::string CArmIPCCMD::DEV_RESET_RESP("DeviceResetResp");

const std::string CArmIPCCMD::CANCEL_SCAN("CANCEL_SCAN");
const std::string CArmIPCCMD::CANCEL_SCAN_RESP("CANCEL_SCAN_RESP");

const std::string CArmIPCCMD::WARN_RESET("WarnReset");
const std::string CArmIPCCMD::WARN_RESET_RESP("WarnResetResp");

const std::string CArmIPCCMD::LASER_SWITCH("LaserSwitch");
const std::string CArmIPCCMD::LASER_SWITCH_RESP("LaserSwitchResp");

const std::string CArmIPCCMD::SET_KV("SetKV");
const std::string CArmIPCCMD::SET_KV_RESP("SetKVResp");

const std::string CArmIPCCMD::SET_MA("SetMA");
const std::string CArmIPCCMD::SET_MA_RESP("SetMAResp");

const std::string CArmIPCCMD::SET_FPS("SetFPS");
const std::string CArmIPCCMD::SET_FPS_RESP("SetFPSResp");

const std::string CArmIPCCMD::SET_EXPOSURE("SetExposure");
const std::string CArmIPCCMD::SET_EXPOSURE_RESP("SetExposureResp");

const std::string CArmIPCCMD::SET_FRAME_NUM("SetFrameNum");
const std::string CArmIPCCMD::SET_FRAME_NUM_RESP("SetFrameNumResp");

const std::string CArmIPCCMD::SET_HOR_COLL("SetCollimatorH");
const std::string CArmIPCCMD::SET_HOR_COLL_RESP("SetCollimatorHResp");

const std::string CArmIPCCMD::SET_COLL_CONTINUE_MOVE("SET_COLL_CONTINUE_MOVE");
const std::string CArmIPCCMD::SET_COLL_CONTINUE_MOVE_RESP("SET_COLL_CONTINUE_MOVE_RESP");

const std::string CArmIPCCMD::SET_VER_COLL("SetCollimatorV");
const std::string CArmIPCCMD::SET_VER_COLL_RESP("SetCollimatorVResp");

const std::string CArmIPCCMD::MOVE_LIFT_COL("MoveAxisZ");
const std::string CArmIPCCMD::MOVE_LIFT_COL_RESP("MoveAxisZResp");

const std::string CArmIPCCMD::MOVE_X_AXIS("MoveAxisX");
const std::string CArmIPCCMD::MOVE_X_AXIS_RESP("MoveAxisXResp");

const std::string CArmIPCCMD::MOVE_C_ARM("MoveAxisY");
const std::string CArmIPCCMD::MOVE_C_ARM_RESP("MoveAxisYResp");

const std::string CArmIPCCMD::GET_POS_LIFT_COL("GetAxisPosZ");
const std::string CArmIPCCMD::GET_POS_LIFT_COL_RESP("GetAxisPosZResp");

const std::string CArmIPCCMD::GET_POS_X_AXIS("GetAxisPosX");
const std::string CArmIPCCMD::GET_POS_X_AXIS_RESP("GetAxisPosXResp");

const std::string CArmIPCCMD::GET_POS_C_ARM("GetAxisPosY");
const std::string CArmIPCCMD::GET_POS_C_ARM_RESP("GetAxisPosYResp");

const std::string CArmIPCCMD::GET_POS_COLLIMATOR("GET_POS_COLLIMATOR");
const std::string CArmIPCCMD::GET_POS_COLLIMATOR_RESP("GET_POS_COLLIMATOR_RESP");

const std::string CArmIPCCMD::SET_POS_C_ARM("SetAxisPosY");
const std::string CArmIPCCMD::SET_POS_C_ARM_RESP("SetAxisPosYResp");

const std::string CArmIPCCMD::SET_POS_X_AXIS("SetAxisPosX");
const std::string CArmIPCCMD::SET_POS_X_AXIS_RESP("SetAxisPosXResp");

const std::string CArmIPCCMD::SET_POS_LIFT_COL("SetAxisPosZ");
const std::string CArmIPCCMD::SET_POS_LIFT_COL_RESP("SetAxisPosZResp");

const std::string CArmIPCCMD::SET_MAN_C_ARM("SetManuAxisY");
const std::string CArmIPCCMD::SET_MAN_C_ARM_RESP("SetManuAxisYResp");

const std::string CArmIPCCMD::SET_MAN_LIFT_COL("SetManuAxisZ");
const std::string CArmIPCCMD::SET_MAN_LIFT_COL_RESP("SetManuAxisZResp");

const std::string CArmIPCCMD::SET_MAN_X_AXIS("SetManuAxisX");
const std::string CArmIPCCMD::SET_MAN_X_AXIS_RESP("SetManuAxisXResp");

const std::string CArmIPCCMD::GET_DAP_VALUE("GetDAPValue");
const std::string CArmIPCCMD::GET_DAP_VALUE_RESP("GetDAPValueResp");

const std::string CArmIPCCMD::SET_3D_TRACK_DATA("Set3DTrack");
const std::string CArmIPCCMD::SET_3D_TRACK_DATA_RESP("Set3DTrackResp");

const std::string CArmIPCCMD::START_SCAN_2D("StartScan2D");
const std::string CArmIPCCMD::START_SCAN_2D_RESP("StartScan2DResp");

const std::string CArmIPCCMD::START_SCAN_3D("StartScan3D");
const std::string CArmIPCCMD::START_SCAN_3D_RESP("StartScan3DResp");

const std::string CArmIPCCMD::FETCH_3D_TRACK("Get3DTrack");
const std::string CArmIPCCMD::FETCH_3D_TRACK_RESP("Get3DTrackResp");

const std::string CArmIPCCMD::ENABLE_DAP("EnableDAP");
const std::string CArmIPCCMD::ENABLE_DAP_RESP("EnableDAPResp");

const std::string CArmIPCCMD::GET_KV("GetKV");
const std::string CArmIPCCMD::GET_KV_RESP("GetKVResp");

const std::string CArmIPCCMD::GET_MA("GetMA");
const std::string CArmIPCCMD::GET_MA_RESP("GetMAResp");

const std::string CArmIPCCMD::GET_Bulb_Tube_Temp("GET_Bulb_Tube_Temp");
const std::string CArmIPCCMD::GET_Bulb_Tube_Temp_RESP("GET_Bulb_Tube_Temp_RESP");

const std::string CArmIPCCMD::GET_CROSS_LINE_LSR_STS("GetCrossLineLaserStatus");
const std::string CArmIPCCMD::GET_CROSS_LINE_LSR_STS_RESP("GetCrossLineLaserStatusResp");

const std::string CArmIPCCMD::GET_VIEW_RANGE_LSR_STS("GetViewRangeLaserStatus");
const std::string CArmIPCCMD::GET_VIEW_RANGE_LSR_STS_RESP("GetViewRangeLaserStatusResp");

const std::string CArmIPCCMD::GET_MB_TEMP("GetMainboardTemp");
const std::string CArmIPCCMD::GET_MB_TEMP_RESP("GetMainboardTempResp");

const std::string CArmIPCCMD::EXP_BTN_PRESSED("ExpBtnPressed");
const std::string CArmIPCCMD::EXP_BTN_PRESSED_RESP("ExpBtnPressedResp");

const std::string CArmIPCCMD::PEDAL_PRESSED("PedalPressed");
const std::string CArmIPCCMD::PEDAL_PRESSED_RESP("PedalPressedResp");

const std::string CArmIPCCMD::AXIS_MOTION_STATUS_CHGD("AxisMotionStatusChanged");
const std::string CArmIPCCMD::AXIS_MOTION_STATUS_CHGD_RESP("AxisMotionStatusChangedResp");

const std::string CArmIPCCMD::COLLIMATOR_IN_PLACE("CollimatorInPlace");
const std::string CArmIPCCMD::COLLIMATOR_IN_PLACE_RESP("CollimatorInPlaceResp");

const std::string CArmIPCCMD::AXIS_OPER_MODE_CHGD("AxisOperModeChanged");
const std::string CArmIPCCMD::AXIS_OPER_MODE_CHGD_RESP("AxisOperModeChangedResp");

const std::string CArmIPCCMD::LASER_SWITCH_CHGD("LaserSwitchChanged");
const std::string CArmIPCCMD::LASER_SWITCH_CHGD_RESP("LaserSwitchChangedResp");

const std::string CArmIPCCMD::RASTER_STATUS_CHGD("RASTER_STATUS_CHGD");
const std::string CArmIPCCMD::RASTER_STATUS_CHGD_RESP("RASTER_STATUS_CHGD_RESP");

const std::string CArmIPCCMD::AXIS_POS_CHGD("AxisPosChanged");
const std::string CArmIPCCMD::AXIS_POS_CHGD_RESP("AxisPosChangedResp");

const std::string CArmIPCCMD::SEND_NEW_IMAGE("SendNewImage");
const std::string CArmIPCCMD::SEND_NEW_IMAGE_RESP("SendNewImageResp");

const std::string CArmIPCCMD::DEV_RESET_DONE("DeviceResetDone");
const std::string CArmIPCCMD::DEV_RESET_DONE_RESP("DeviceResetDoneResp");

const std::string CArmIPCCMD::ENABLE_TUBE_CTRL("EnableTubeCtrl");
const std::string CArmIPCCMD::ENABLE_TUBE_CTRL_RESP("EnableTubeCtrlResp");

const std::string CArmIPCCMD::RADIARION_PERMISSION_CTRL("RADIARION_PERMISSION_CTRL");
const std::string CArmIPCCMD::RADIARION_PERMISSION_CTRL_RESP("RADIARION_PERMISSION_CTRL_RESP");

const std::string CArmIPCCMD::GET_BATTERY_PERCENT("GET_BATTERY_PERCENT");
const std::string CArmIPCCMD::GET_BATTERY_PERCENT_RESP("GET_BATTERY_PERCENT_RESP");

const std::string CArmIPCCMD::GET_RASTER_STATUS("GET_RASTER_STATUS");
const std::string CArmIPCCMD::GET_RASTER_STATUS_RESP("GET_RASTER_STATUS_RESP");

const std::string CArmIPCCMD::SLAVE_SCAN_CTRL("SLAVE_SCAN_CTRL");
const std::string CArmIPCCMD::SLAVE_SCAN_CTRL_RESP("SLAVE_SCAN_CTRL_RESP");