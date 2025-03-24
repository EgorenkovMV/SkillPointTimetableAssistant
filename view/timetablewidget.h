#ifndef TIMETABLEWIDGET_H
#define TIMETABLEWIDGET_H

#include <QWidget>
#include <QCalendarWidget>
#include "../model/timetablemanager.h"
#include "../view/daytimetableinfo.h"


class TimetableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimetableWidget(QWidget *parent = nullptr, std::shared_ptr<TimetableManager> ttmng = nullptr);

signals:

private:
    std::shared_ptr<TimetableManager> ttmng;
    QCalendarWidget *calendar;
    DayTimetableInfo *dayInfo;
};

#endif // TIMETABLEWIDGET_H
