#include "MyCalendarWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCharFormat>
#include <QToolButton>
#include <QLocale>
#include <QSpinBox>

#if _MSC_VER >= 1600

#pragma execution_character_set("utf-8")

#endif

MyCalendarWidget::MyCalendarWidget(QWidget* w,QLineEdit* e, QString iconPath)
	: QFrame(w)
	, m_calendar(new QCalendarWidget(this)),
	currentIconPath(iconPath)
{
	initUI();
	initLogic();
	initStyle();
	dateEdit = e;
}

MyCalendarWidget::~MyCalendarWidget()
{
}

void MyCalendarWidget::setLineEditDate()
{
	if (dateEdit != nullptr)
	{
		QDate date = m_calendar->selectedDate();
		QString dtstr = date.toString("yyyy-MM-dd");
		dateEdit->setText(dtstr);
		this->close();
	}
}

void MyCalendarWidget::PageChanged(int year, int month)
{
	int date = QDate::currentDate().day();
	m_calendar->setSelectedDate(QDate(year, month, date));
}

void MyCalendarWidget::initUI()
{
	m_calendar->setMinimumDate(QDate(1900, 1, 1));
	m_calendar->setMaximumDate(QDate::currentDate());
	m_calendar->setGridVisible(false); //禁用网格
	m_calendar->setVerticalHeaderFormat(QCalendarWidget::VerticalHeaderFormat::NoVerticalHeader);
	m_calendar->setHorizontalHeaderFormat(QCalendarWidget::HorizontalHeaderFormat::ShortDayNames);
	m_calendar->setSelectionMode(QCalendarWidget::SingleSelection);


	if (m_caltrans == "english")
		m_calendar->setLocale(QLocale(QLocale::English, QLocale::Armenia));
	else if (m_caltrans == "chinese")
		m_calendar->setLocale(QLocale(QLocale::Chinese, QLocale::China));

	m_calendar->setSelectedDate(QDate::currentDate());


	QHBoxLayout* btnLayout = new QHBoxLayout();
	btnLayout->setContentsMargins(0, 0, 0, 0);


	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(m_calendar);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	this->setAttribute(Qt::WA_DeleteOnClose); 
	this->setLayout(mainLayout);
	this->setWindowModality(Qt::ApplicationModal);
	this->setWindowFlags(Qt::Popup);
}

void MyCalendarWidget::initLogic()
{
	connect(m_calendar, SIGNAL(currentPageChanged(int, int)), this, SLOT(PageChanged(int, int)));
	connect(m_calendar, SIGNAL(clicked(const QDate &)), this, SLOT(setLineEditDate()));
}

void MyCalendarWidget::setMinMaxDate(QDate minDate, QDate maxDate)
{
    m_calendar->setMinimumDate(minDate);
    m_calendar->setMaximumDate(maxDate);
}

void MyCalendarWidget::initStyle()
{
	m_calendar->setFixedSize(250, 250);

	m_calendar->setStyleSheet("QCalendarWidget QWidget#qt_calendar_navigationbar{background-color:#181B1F; font: normal 12px  Microsoft YaHei;} \
		QCalendarWidget QToolButton{color:#FFFFFF;icon-size:25px,25px;background-color:#181B1F;font: normal 12px  Microsoft YaHei; } \
		QCalendarWidget QMenu{background-color:#282f35;color:#FFFFFF; padding:0px 0px;margin:0px 0px;font: normal 12px  Microsoft YaHei;}  \
		QCalendarWidget QMenu::item{background-color: #282f35;border-bottom:0px solid #D3D3D3;}\
		QCalendarWidget QMenu::item:selected{background-color: #181B1F;}\
		QCalendarWidget QMenu::item:disabled{background-color: #282f35; color: rgb(64,64,64);}\
		QCalendarWidget QSpinBox{color:#FFFFFF;background-color:qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #4a525a,stop:1 #768699);selection-background-color:#282f35;selection-color:#FFFFFF; font: normal 12px  Microsoft YaHei;} \
		QCalendarWidget QSpinBox::up-button{subcontrol-origin:border;subcontrol-position:top right;width:20px; height:12px} \
		QCalendarWidget QSpinBox::down-button{subcontrol-origin:border;subcontrol-position:bottom right;width:20px;height:12px} \
		QCalendarWidget QSpinBox::up-arrow{width:10px;height:10px;} \
		QCalendarWidget QSpinBox::down-arrow{width:10px;height:10px;} \
		QCalendarWidget QWidget{alternate-background-color:#282f35;} \
		QCalendarWidget QAbstractItemView:enabled{color:#FFFFFF;background-color:#282f35;selection-background-color:#454f5b;selection-color:#FFFFFF; font: normal 12px  Microsoft YaHei;} \
		QCalendarWidget QAbstractItemView:disabled{color:rgb(64,64,64); }");//
	QTextCharFormat f = m_calendar->weekdayTextFormat(Qt::Monday);
	f.setForeground(QBrush(QColor("#FFFFFF")));
	QTextCharFormat weekdays = f;
	m_calendar->setWeekdayTextFormat(Qt::Sunday, weekdays);
	m_calendar->setWeekdayTextFormat(Qt::Saturday, weekdays);

	QToolButton* prevBtn = m_calendar->findChild<QToolButton*>(QLatin1String("qt_calendar_prevmonth"));

	QToolButton* nextBtn = m_calendar->findChild<QToolButton*>(QLatin1String("qt_calendar_nextmonth"));

	QString preIconPath = currentIconPath + "/go-previous.png";
	QString nextIconPath = currentIconPath + "/go-next.png";
	prevBtn->setIcon(QIcon(preIconPath));
	nextBtn->setIcon(QIcon(nextIconPath));

	QSpinBox* yearEdit = m_calendar->findChild<QSpinBox*>(QLatin1String("qt_calendar_yearedit"));
	yearEdit->setContextMenuPolicy(Qt::NoContextMenu);
}

void MyCalendarWidget::mousePressEvent(QMouseEvent * e)
{
    setAttribute(Qt::WA_NoMouseReplay);
    QWidget::mousePressEvent(e);
}