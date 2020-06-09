#ifndef CARMDATA_H_
#define CARMDATA_H_

/**
* @file CArmData.h
* @brief 小C触屏数据结构文件
* @author 路旺明
*/
#include <QString>

typedef struct
{
    const QString widgetStyle1 = "background-color:rgba(14,14,14,1)";
    const QString widgetStyle2 = "background-color:rgba(20,25,28,1)";
    const QString lineStyle = "background-color:rgba(255,255,255,0.1)";
    const QString currentTab = "background-color:rgba(0,145, 255, 1)";
    const QString toolButton = "QToolButton{background-color:rgba(255,255,255,0.1);border:0px solid #303840;border-radius:2px;color:#E1E8F1;font:16px Microsoft YaHei;\
		padding-top:2;padding-left:66;}";
    const QString transferFalse = ":/CArmTouchPanel/Resource/transfer.png";
    const QString backImage = ":/CArmTouchPanel/Resource/goBack.png";
    const QString transferTrue = ":/CArmTouchPanel/Resource/transferTrue.png";
    const QString UEGLabel = ":/CArmTouchPanel/Resource/logo.png";
    const QString scanLabelTrue = ":/CArmTouchPanel/Resource/scanTrue.png";
    const QString scanLabelFalse = ":/CArmTouchPanel/Resource/scanFalse.png";
    const QString imageTabTrue = ":/CArmTouchPanel/Resource/imageChangeTrue.png";
    const QString imageTbaFalse = ":/CArmTouchPanel/Resource/imageChangeFalse.png";
    const QString scaningFalse = ":/CArmTouchPanel/Resource/scaningFalse.png";
    const QString listNum = ":/CArmTouchPanel/Resource/listNum.png";
    const QString videoimage = ":/CArmTouchPanel/Resource/cine.png";
    const QString scaningTrue = ":/CArmTouchPanel/Resource/scaningTrue.png";
    const QString GArmFalse = ":/CArmTouchPanel/Resource/GArmFalse.png";
    const QString GArmTrue = ":/CArmTouchPanel/Resource/GArmTrue.png";
    const QString x_ray = ":/statusBar/Resource/statusBar/X-ray.png";
    const QString video = ":/statusBar/Resource/statusBar/video.png";
    const QString pointCapture = ":/statusBar/Resource/statusBar/PointTaken.png";

    const QString normalDose = ":/statusBar/Resource/statusBar/NormalDose.png";
    const QString lowDose = ":/statusBar/Resource/statusBar/lowDose.png";
    const QString highQuality = ":/statusBar/Resource/statusBar/highQuality.png";


    const QString armAndLegs = ":/statusBar/Resource/statusBar/armsAndLegs.png";
    const QString neck = ":/statusBar/Resource/statusBar/neck.png";
    const QString head = ":/statusBar/Resource/statusBar/head.png";
    const QString trunk = ":/statusBar/Resource/statusBar/trunk.png";
    const QString heart = ":/statusBar/Resource/statusBar/heart.png";
    const QString abdomen = ":/statusBar/Resource/statusBar/abdomen.png";
    const QString urinary = ":/statusBar/Resource/statusBar/urinary.png";
    const QString flab = ":/statusBar/Resource/statusBar/flab.png";
    const QString endoscope = ":/statusBar/Resource/statusBar/endoscope.png";

    
    const QString patientHead = ":/CArmTouchPanel/Resource/patientHead.png";
    const QString GArm = ":/statusBar/Resource/statusBar/GArm.png";

    const QString contrast = ":/imageOperation/Resource/imageOperation/contrastIcon.png";
    const QString brightness = ":/imageOperation/Resource/imageOperation/brightnessIcon.png";
    const QString metal = ":/imageOperation/Resource/imageOperation/metaliIcon.png";
    const QString sharpen = ":/imageOperation/Resource/imageOperation/sharpenIcon.png";
    const QString denoise = ":/imageOperation/Resource/imageOperation/denoiseIcon.png";

    const QString metaReviseFalse = ":/imageOperation/Resource/imageOperation/metalFalse.png";
    const QString metaReviseTrue = ":/imageOperation/Resource/imageOperation/metalTrue.png";
    const QString negativeFalse = ":/imageOperation/Resource/imageOperation/negativeFalse.png";
    const QString negativeTrue = ":/imageOperation/Resource/imageOperation/negativeTrue.png";
    const QString rotateFalse = ":/imageOperation/Resource/imageOperation/rotateFalse.png";
    const QString rotateTrue = ":/imageOperation/Resource/imageOperation/rotateTrue.png";
    const QString levelFalse = ":/imageOperation/Resource/imageOperation/levelFalse.png";
    const QString levelTrue = ":/imageOperation/Resource/imageOperation/levelTrue.png";
    const QString verticalFalse = ":/imageOperation/Resource/imageOperation/verticalFalse.png";
    const QString verticalTrue = ":/imageOperation/Resource/imageOperation/verticalTrue.png";
    const QString reset = ":/imageOperation/Resource/imageOperation/resetImage.png";

    const QString checkHistory = "QPushButton{background:transparent;image:url(:/CArmTouchPanel/Resource/historytrue.png);\
background:rgba(0, 0, 0, 0);opacity:0.7;};"
    "QPushButton:checked{background:transparent;image:url(:/CArmTouchPanel/Resource/historyFalse.png);}";
   
    const QString scrollAareBar = "QScrollBar:vertical"
        "{"
        "width:8px;"
        "background:rgb(20,25,28);"
        "margin:0px,0px,0px,0px;"
        "padding-top:9px;"
        "padding-bottom:9px;"
        "}"
        "QScrollBar::handle:vertical"
        "{"
        "width:8px;"
        "background-color:rgba(178,184,190,0.5);"
        " border-radius:4px;"
        "min-height:20;"
        "}"
        "QScrollBar::handle:vertical:hover"
        "{"
        "width:8px;"
        "background-color:rgba(178,184,190,0.5);"
        " border-radius:4px;"
        "min-height:20;"
        "}"
        "QScrollBar::add-line:vertical"
        "{"
        "height:9px;width:8px;"
        "border-image:url(:/images/a/3.png);"
        "subcontrol-position:bottom;"
        "}"
        "QScrollBar::sub-line:vertical"
        "{"
        "height:9px;width:8px;"
        "border-image:url(:/images/a/1.png);"
        "subcontrol-position:top;"
        "}"
        "QScrollBar::add-line:vertical:hover"
        "{"
        "height:9px;width:8px;"
        "subcontrol-position:bottom;"
        "}"
        "QScrollBar::sub-line:vertical:hover"
        "{"
        "height:9px;width:8px;"
        "subcontrol-position:top;"
        "}"
        "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
        "{"
        "background:rgba(0,0,0,10%);"
        "border-radius:4px;"
        "}";

    const QString kvpLabel = "QLabel{background-color:rgba(20,25,28,1);font:14px Microsoft YaHei; color:rgba(180,196,217,0.7);}";
    const QString valueLabel = "QLabel{background-color:rgba(20,25,28,1);font:21px Microsoft YaHei; color:rgba(177,193,214,1);}";
    const QString pulse = "QLabel{background-color:rgba(20,25,28,1);font:18px Microsoft YaHei; color:rgba(180,196,217,0.7);}";
    const QString nameLabel = "QLabel{font:16px Microsoft YaHei; color:rgba(255,255,255,1);}";
    const QString metalLabelFalse = "QLabel{background-color:rgba(255,255,255,0.1); border-radius:0px;font:16px Microsoft YaHei; color:rgba(255,255,255,0.4);}";
    const QString metalLabelTrue = "QLabel{background-color:rgba(0,145,255,1); border-radius:0px 0px 0px 2px;font:16px Microsoft YaHei; color:rgba(255,255,255,1);}";
    const QString gear = "font:12px Microsoft YaHei; color:rgba(177,193, 214,1)";
    

    const QString imageLabel = "QLabel{background-color:rgba(11,209,140,0.25);border-radius:1px;font:11px Microsoft YaHei; color:rgba(11,209,140,1);}";
    const QString labelFont = "font:12px Microsoft YaHei; color:rgba(177,193,214,1)";
    const QString rotateFont = "QLabel{background:transparent;font:11px Microsoft YaHei; color:rgba(177,193,214,1);}";
    const QString text = "font:14px Microsoft YaHei; color:rgba(180,196,217,0.7);";
    const QString text1 = "font:12px Microsoft YaHei; color:rgba(110,114,132,1);";
    const QString slider = "QSlider::groove:horizontal{height: 4px;background-color:rgba(66,80,96,1) ;border: 1px solid #566576;border-radius: 0px;padding - left:-1px;padding right:-1px;}"
        "QSlider::sub-page:horizontal{height: 4px;background: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 #0091FF, stop:1 #0091FF);background: qlineargradient(x1 : 0, y1 : 0.2, x2 : 1, y2 : 1, stop : 0 #0091FF, stop:1 #0091FF);\
    border: 1px solid #4A708B;\
        border - radius: 2px;\
    }"
    "QSlider::handle:horizontal\
    {\
    width: 22px;\
        background-color:rgba(255,255,255,1);\
margin-top: -10px;\
margin-bottom: -10px;\
border-radius: 5px;\
    }";

}CARM_STYLE_STRUCT;

typedef struct
{
    const QString widgetStyle = "QWidget{background:rgba(14,14,14,1);}";
    const QString captureModeStyle = "QPushButton{background:rgba(0,0,0,0); opacity:0.7;}";

    const QString limbsStyle = "QPushButton{image:url(:/modelParts/Resource/modelParts/limbs.png)}";
    const QString neckStyle = "QPushButton{image:url(:/modelParts/Resource/modelParts/neck.png)}";
    const QString headStyle = "QPushButton{image:url(:/modelParts/Resource/modelParts/head.png)}";
    const QString trunkStyle = "QPushButton{image:url(:/modelParts/Resource/modelParts/trunk.png)}";
    const QString heartStyle = "QPushButton{image:url(:/modelParts/Resource/modelParts/heart.png)}";
    const QString abdomenStyle = "QPushButton{image:url(:/modelParts/Resource/modelParts/abdomen.png)}";
    const QString urinaryStyle = "QPushButton{image:url(:/modelParts/Resource/modelParts/urinary.png)}";
    const QString soft_tissueStyle = "QPushButton{image:url(:/modelParts/Resource/modelParts/soft_tissue.png)}";
    const QString endoscopeStyle = "QPushButton{image:url(:/modelParts/Resource/modelParts/endoscope.png)}";

    const QString limbsStyle_check = "QPushButton{image:url(:/modelParts/Resource/modelParts/limbs_checked.png)}";
    const QString neckStyle_check = "QPushButton{image:url(:/modelParts/Resource/modelParts/neck_checked.png)}";
    const QString headStyle_check = "QPushButton{image:url(:/modelParts/Resource/modelParts/head_checked.png)}";
    const QString trunkStyle_check = "QPushButton{image:url(:/modelParts/Resource/modelParts/trunk_checked.png)}";
    const QString heartStyle_check = "QPushButton{image:url(:/modelParts/Resource/modelParts/heart_checked.png)}";
    const QString abdomenStyle_check = "QPushButton{image:url(:/modelParts/Resource/modelParts/abdomen_checked.png)}";
    const QString urinaryStyle_check = "QPushButton{image:url(:/modelParts/Resource/modelParts/urinary_checked.png)}";
    const QString soft_tissueStyle_check = "QPushButton{image:url(:/modelParts/Resource/modelParts/soft_tissue_checked.png)}";
    const QString endoscopeStyle_check = "QPushButton{image:url(:/modelParts/Resource/modelParts/endoscope_checked.png)}";

    const QString underFrameLeft = "QLabel{image:url(:/modelParts/Resource/modelParts/underframeL.png)}";
    const QString underFrameMiddle = "QLabel{image:url(:/modelParts/Resource/modelParts/underframeM.png)}";
    const QString underFrameRight = "QLabel{image:url(:/modelParts/Resource/modelParts/underframeR.png)}";

    const QString frameBorder = "QFrame{border:1px solid rgba(151,151,151,1); border-radius:3px}";
    const QString PPSfontStyle = "QPushButton{background:transparent; font - size:14px;"
        "font - family:NotoSansCJK - Regular, NotoSansCJK;"
        " font - weight:400;color:rgba(180, 196, 217, 1); line - height:21px; }";

    const QString pointCap = "QPushButton{ background:rgba(0, 0, 0, 0);"
        "opacity:0.7; image:url(:/modelParts/Resource/modelParts/point_capture_cheched.png);}";
    const QString perspecCap = "QPushButton{ background:rgba(0, 0, 0, 0);"
        "opacity:0.7; image:url(:/modelParts/Resource/modelParts/pulse_perspective_checked.png;}";
    const QString videoCap =  "QPushButton{ background:rgba(0, 0, 0, 0);"
        "opacity:0.7; image:url(:/modelParts/Resource/modelParts/cine_checked.png;}";
    


}SCANSETTING_STYLE_STRUCT;

typedef enum
{
    SCAN_TAB = 0,
    IMAGE_TAB,
    GARM_TAB
}CARM_TAB_ENUM;

typedef enum 
{
    ///智能金属校正
    CARM_OPERATION_METAL_REVISE = 0,

    ///负片
    CARM_OPERATION_NEGATIVE,

    ///旋转
    CARM_OPERATION_ROTATE,

    ///水平翻转
    CARM_OPERATION_LEVEL,

    ///垂直翻转
    CARM_OPERATION_VERTICAL,

    ///重置图像
    CARM_OPERATION_RESET


}CARM_IMAGE_OPERATION_ENUM;

typedef enum
{
    METAL_ZERO_GEAR = 0,
    METAL_ONE_GEAR,
    METAL_TWO_GEAR,
    METAL_THREE_GEAR
}CARM_METAL_GEAR_ENUM;

typedef enum 
{
    CRAM_GEAR_ONE = 1,
    CRAM_GEAR_TWO,
    CRAM_GEAR_THREE
}CARM_GEAR_ENUM;

#endif // !CARMDATA_H_

