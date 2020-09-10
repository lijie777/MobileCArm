#include "CArmHistoryWidget.h"

CArmHistoryWidget::CArmHistoryWidget(QWidget *parent)
    :QWidget(parent)
    , m_iMppCount(0)
    , m_iVideoCount(0)
    , m_iPointCount(0)
    , m_bHistoryLoad(false)
{
    setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    checkButton->setStyleSheet(m_styleStruct.checkHistory);
    returnHistory->setStyleSheet(m_styleStruct.toolButton);
   // checkButton->setEnabled(false);
    returnHistory->setIcon(QIcon(QString::fromLocal8Bit(m_styleStruct.backImage.toLatin1().data())));
    returnHistory->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    historyStackedWidget->setStyleSheet("background-color:rgba(0,0,0,1)");
    historyStackedWidget->setCurrentIndex(0);

    /*历史图像列表*/
    historyTableWidget->verticalScrollBar()->setStyleSheet(m_styleStruct.scrollAareBar);
    historyTableWidget->setColumnCount(1);
    historyTableWidget->setColumnWidth(0, 150);
    /*设置表格开始没有选择*/
    historyTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    /*防止表格点击有虚线*/
    historyTableWidget->setFocusPolicy(Qt::NoFocus);
    /*左表头和上表头不显示*/
    QHeaderView *hHeader = historyTableWidget->verticalHeader();
    QHeaderView *vHeader = historyTableWidget->horizontalHeader();
    hHeader->setHidden(true);
    vHeader->setHidden(true);

    /*序列图像列表*/
    serialTableWidget->verticalScrollBar()->setStyleSheet(m_styleStruct.scrollAareBar);
    serialTableWidget->setColumnCount(1);
    serialTableWidget->setColumnWidth(0, 150);


    /*设置表格开始没有选择*/
    serialTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    /*防止表格点击有虚线*/
    serialTableWidget->setFocusPolicy(Qt::NoFocus);
    /*左表头和上表头不显示*/
    QHeaderView *hSerialHeader = serialTableWidget->verticalHeader();
    QHeaderView *vSerialHeader = serialTableWidget->horizontalHeader();
    hSerialHeader->setHidden(true);
    vSerialHeader->setHidden(true);

    m_ImageModeDataMap.clear();

    connect(historyTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(imageListClicked_slot(int, int)));
    connect(serialTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(serialListClicked_slot(int, int)));
    connect(checkButton, SIGNAL(clicked()), this, SLOT(checkButton_slot()));
    connect(returnHistory, SIGNAL(clicked()), this, SLOT(returnHistory_slot()));

}

CArmHistoryWidget::~CArmHistoryWidget()
{

}

void CArmHistoryWidget::checkButton_slot()
{
    static bool flag = true;
    if (flag == true)
    {
        emit sendCheckButtonStatus(flag);
        flag = false;
    }
    else
    {
        emit sendCheckButtonStatus(flag);
        flag = true;
    }
}

void CArmHistoryWidget::returnHistory_slot()
{
    historyStackedWidget->setCurrentIndex(0);
}

void CArmHistoryWidget::imageListClicked_slot(int row, int cl)
{
    if (historyTableWidget->cellWidget(row, cl)->objectName() == "MPP")
    {
        /*已经加载过了*/
        if (m_serialImageDataVec.size() == serialTableWidget->rowCount())
        {
            return;
        }
        historyStackedWidget->setCurrentIndex(1);
        setSerialImageList();
       
    }
    else if(historyTableWidget->cellWidget(row, cl)->objectName() == "POINT")
    {
        m_currentData = qobject_cast<CArmHistoryImageView *>(historyTableWidget->cellWidget(row, cl))->getReviewData();

    }
    else if(historyTableWidget->cellWidget(row, cl)->objectName() == "VIDEO")
    {
        m_currentData = qobject_cast<CArmHistoryImageView *>(historyTableWidget->cellWidget(row, cl))->getReviewData();
    }
}

void CArmHistoryWidget::serialListClicked_slot(int row, int cl)
{
    if (serialTableWidget->cellWidget(row, cl)->objectName() == "serial")
    {
        m_currentData = qobject_cast<CArmHistoryImageView *>(serialTableWidget->cellWidget(row, cl))->getReviewData();
    }
}

void CArmHistoryWidget::clearImageHistoryList()
{
    historyTableWidget->clear();
    serialTableWidget->clear();
    m_ImageModeDataMap.clear();
    m_iMppCount = 0;
    m_iVideoCount = 0;

}

void CArmHistoryWidget::setMppHistoryImageData(std::vector<CP_ReviewData*> mppImageData)
{
    m_serialImageDataVec = mppImageData;
    if (historyTableWidget->rowCount() == 0)
    {
        setMPPImageList(mppImageData);
        m_bHistoryLoad = true;
    }
    else
    {
        if (m_bHistoryLoad == false)
        {
            setMPPImageList(mppImageData);
        }
    }
}


void CArmHistoryWidget::setVideoImageData( std::vector<CP_ReviewData*> voidImageData)
{

    historyTableWidget->insertRow(historyTableWidget->rowCount());

    historyTableWidget->setRowHeight(historyTableWidget->rowCount() - 1, 150);

    CArmHistoryImageView *imageView = new CArmHistoryImageView(IMAGE_LIST_VIEW, this);
    QGridLayout *imageLayout = new QGridLayout;
    QWidget *viewWidget = new QWidget;

    viewWidget->setObjectName(tr("VIDEO"));
    if (imageView != nullptr)
    {
        imageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        imageLayout->addWidget(imageView);
        imageLayout->setMargin(0);
        imageLayout->setSpacing(0);
        viewWidget->setLayout(imageLayout);
        IImage2DTextureImportorComponentPtr pTextureImportor;

        pTextureImportor = ViewToolApi::createImage2DTextureImporter(imageView);
        TextureObject3DPtr texture = CArmCommon::CreateTextureObject(voidImageData[0]->_imageData);
        voidImageData[0]->_texture = texture;
        if (pTextureImportor.createTexture(texture) == true)
        {
            imageView->loadReviewData(voidImageData[0]);
        }
        historyTableWidget->setCellWidget(historyTableWidget->rowCount() - 1, 0, viewWidget);
        imageView->show();

        QLabel *numLabe = new QLabel(viewWidget);
        numLabe->setFixedSize(QSize(20, 20));
        numLabe->setStyleSheet("QLabel{background:transparent; font-size:14px Microsoft YaHei; color:rgba(255,255,255,1);}");
        numLabe->setAlignment(Qt::AlignCenter);
        numLabe->move(120, 0);
        numLabe->setText(QString("%1").arg(historyTableWidget->rowCount()));
        numLabe->show();

        QLabel *textLabel = new QLabel(viewWidget);
        textLabel->setStyleSheet("QLabel{background:transparent; font-size:14px Microsoft YaHei; color:rgba(255,255,255,1);}");
        textLabel->setFixedSize(QSize(42, 42));
        QPixmap map(m_styleStruct.videoimage);
        map.scaled(textLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        textLabel->setPixmap(map);
        textLabel->setAlignment(Qt::AlignCenter);
        textLabel->move(103, 103);
        textLabel->show();
    }
}

void CArmHistoryWidget::setPointImageData(std::vector<CP_ReviewData*> pointImageData)
{
    historyTableWidget->insertRow(historyTableWidget->rowCount());

    historyTableWidget->setRowHeight(historyTableWidget->rowCount() - 1, 150);

    CArmHistoryImageView *imageView = new CArmHistoryImageView(IMAGE_LIST_VIEW, this);
    QGridLayout *imageLayout = new QGridLayout;
    QWidget *viewWidget = new QWidget;

    viewWidget->setObjectName(tr("MPP"));
    if (imageView != nullptr)
    {
        imageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        imageLayout->addWidget(imageView);
        imageLayout->setMargin(0);
        imageLayout->setSpacing(0);
        viewWidget->setLayout(imageLayout);
        IImage2DTextureImportorComponentPtr pTextureImportor;

        pTextureImportor = ViewToolApi::createImage2DTextureImporter(imageView);
        TextureObject3DPtr texture = CArmCommon::CreateTextureObject(pointImageData[m_iPointCount]->_imageData);
        pointImageData[m_iPointCount]->_texture = texture;
        if (pTextureImportor.createTexture(texture) == true)
        {
            imageView->loadReviewData(pointImageData[m_iPointCount]);
        }
        historyTableWidget->setCellWidget(historyTableWidget->rowCount() - 1, 0, viewWidget);
        imageView->show();

        QLabel *numLabe = new QLabel(viewWidget);
        numLabe->setFixedSize(QSize(20, 20));
        numLabe->setStyleSheet("QLabel{background:transparent; font-size:14px Microsoft YaHei; color:rgba(255,255,255,1);}");
        numLabe->setAlignment(Qt::AlignCenter);
        numLabe->move(120, 0);
        numLabe->setText(QString("%1").arg(historyTableWidget->rowCount()));
        numLabe->show();
    }
    m_iPointCount++;
}

void CArmHistoryWidget::setMPPImageList(std::vector<CP_ReviewData*> mppImageData)
{
    historyTableWidget->insertRow(historyTableWidget->rowCount());

    historyTableWidget->setRowHeight(historyTableWidget->rowCount() - 1, 150);

    CArmHistoryImageView *imageView = new CArmHistoryImageView(IMAGE_LIST_VIEW, this);
    QGridLayout *imageLayout = new QGridLayout;
    QWidget *viewWidget = new QWidget;

    viewWidget->setObjectName(tr("POINT"));
    if (imageView != nullptr)
    {
        imageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        imageLayout->addWidget(imageView);
        imageLayout->setMargin(0);
        imageLayout->setSpacing(0);
        viewWidget->setLayout(imageLayout);
        IImage2DTextureImportorComponentPtr pTextureImportor;

        pTextureImportor = ViewToolApi::createImage2DTextureImporter(imageView);
        TextureObject3DPtr texture = CArmCommon::CreateTextureObject(mppImageData[0]->_imageData);
        mppImageData[0]->_texture = texture;
        if (pTextureImportor.createTexture(texture) == true)
        {
            imageView->loadReviewData(mppImageData[0]);
        }
        historyTableWidget->setCellWidget(historyTableWidget->rowCount() - 1, 0, viewWidget);
        imageView->show();
    }

    QLabel *upText = new QLabel(viewWidget);
    upText->setFixedSize(QSize(20, 20));
    upText->setStyleSheet("QLabel{background:transparent; font-size:14px Microsoft YaHei; color:rgba(255,255,255,1);}");
    upText->setAlignment(Qt::AlignCenter);
    upText->move(120, 0);
    upText->setText(QString("%1").arg(historyTableWidget->rowCount()));
    upText->show();


    QLabel *textLabel = new QLabel(viewWidget);
    textLabel->setStyleSheet("QLabel{background:transparent; font-size:14px Microsoft YaHei; color:rgba(255,255,255,1);}");
    textLabel->setFixedSize(QSize(42, 42));
    QPixmap map(m_styleStruct.listNum);
    map.scaled(textLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    textLabel->setPixmap(map);
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->move(103, 103);
    textLabel->show();

    QLabel *numLabel = new QLabel(textLabel);
    numLabel->setFixedSize(QSize(30, 30));
    numLabel->setStyleSheet("QLabel{background:transparent; font-size:14px Microsoft YaHei; color:rgba(20,25,28,1);}");
    numLabel->setAlignment(Qt::AlignCenter);
    numLabel->setText(QString("%1").arg(mppImageData.size()));
    numLabel->move(6, 6);
    numLabel->show();

    m_iMppCount++;
}

void CArmHistoryWidget::setSerialImageList()
{

    int row = 1;
    for (auto serial : m_serialImageDataVec)
    {
        serialTableWidget->insertRow(serialTableWidget->rowCount());

        serialTableWidget->setRowHeight(serialTableWidget->rowCount() - 1, 150);

        CArmHistoryImageView *imageView = new CArmHistoryImageView(IMAGE_LIST_VIEW, this);
        QGridLayout *imageLayout = new QGridLayout;
        QWidget *viewWidget = new QWidget;
        viewWidget->setObjectName(tr("serial"));
        if (imageView != nullptr)
        {
            imageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            imageLayout->addWidget(imageView);
            imageLayout->setMargin(0);
            imageLayout->setSpacing(0);
            viewWidget->setLayout(imageLayout);
            IImage2DTextureImportorComponentPtr pTextureImportor;

            pTextureImportor = ViewToolApi::createImage2DTextureImporter(imageView);
            TextureObject3DPtr texture = CArmCommon::CreateTextureObject(serial->_imageData);
            serial->_texture = texture;
            if (pTextureImportor.createTexture(texture) == true)
            {
                imageView->loadReviewData(serial);
            }
            serialTableWidget->setCellWidget(serialTableWidget->rowCount() - 1, 0, viewWidget);
            imageView->show();
        }

        QLabel *upText = new QLabel(viewWidget);
        upText->setFixedSize(QSize(20, 20));
        upText->setStyleSheet("QLabel{background:transparent; font-size:14px Microsoft YaHei; color:rgba(255,255,255,1);}");
        upText->setAlignment(Qt::AlignCenter);
        upText->move(120, 0);
        upText->setText(QString("%1").arg(row));
        upText->show();
        row++;
    }
}


