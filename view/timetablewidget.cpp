#include "timetablewidget.h"
#include <QHBoxLayout>
#include <QPushButton>

TimetableWidget::TimetableWidget(QWidget *parent, std::shared_ptr<TimetableManager> ttmng)
    : QWidget{parent}
    , ttmng(ttmng)
    , calendar {new QCalendarWidget()}
    , dayInfo {new DayTimetableInfo(nullptr, ttmng)}

{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(calendar);
    layout->addWidget(dayInfo);
    layout->setStretch(0, 1);
    layout->setStretch(1, 1);

    calendar->setFirstDayOfWeek(Qt::DayOfWeek::Monday);
    calendar->setCurrentPage(QDate::currentDate().year(), QDate::currentDate().month());
    calendar->setGridVisible(true);

    connect(calendar, &QCalendarWidget::selectionChanged, this, [this] () {
        this->dayInfo->setDay(this->calendar->selectedDate());
    });

}
