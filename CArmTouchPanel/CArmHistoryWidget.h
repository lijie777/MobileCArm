#pragma once
#include "ui_CArmHistoryImageWidget.h"
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include "CArmData.h"
#include "TextureData.h"
#include "CArmHistoryImageView.h"
#include "../Common/Include/CArmCommon.h"
#include "../Common/Include/Config.h"
#include "ViewToolCreator.h"
#include <QScrollBar>
enum ViewType
{
    IMAGE_LIST_VIEW = 0,
};
class CArmHistoryWidget : public QWidget, public Ui::historyWidget
{
    Q_OBJECT
public:
    CArmHistoryWidget(QWidget *parent = nullptr);
    ~CArmHistoryWidget();


    void clearImageHistoryList();

    void setMppHistoryImageData(std::vector<CP_ReviewData*>);

    void setVideoImageData(std::vector<CP_ReviewData*>);

    void setPointImageData(std::vector<CP_ReviewData*>);

    inline CP_ReviewData getCurrentImageData() {
        return m_currentData
            ;
    };

private slots:
    void checkButton_slot();
    void returnHistory_slot();
    void imageListClicked_slot(int, int);
    void serialListClicked_slot(int, int);
signals:
    void sendCheckButtonStatus(bool state);
    void sendChangedImageIndex(int);

private:
    void setMPPImageList(std::vector<CP_ReviewData*>);

    void setSerialImageList();

private:
    CARM_STYLE_STRUCT m_styleStruct;

    bool       m_bHistoryLoad;

    std::vector<CP_ReviewData*> m_serialImageDataVec;

    CP_ReviewData m_currentData;
    int m_iMppCount;
    int m_iVideoCount;
    int m_iPointCount;
    std::map<ScanMode, std::vector<CP_ImageData> > m_ImageModeDataMap;
};