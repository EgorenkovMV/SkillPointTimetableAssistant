#ifndef DAYTIMETABLEINFO_H
#define DAYTIMETABLEINFO_H

#include <QWidget>
#include <QFrame>
#include <QDate>
#include <QTime>
#include <QLayout>
#include "../model/timetablemanager.h"

class LessonTile : public QFrame
{
    Q_OBJECT
public:
    explicit LessonTile(QWidget *parent = nullptr, const std::shared_ptr<Lesson> &lesson = nullptr);

private:
    std::shared_ptr<Lesson> lesson;
    static QVector<QString> colorCycleNames;
    static int colorCycleCounter;

};

class DayTimetableInfo : public QWidget
{
    Q_OBJECT
public:
    explicit DayTimetableInfo(QWidget *parent = nullptr, const std::shared_ptr<TimetableManager> &ttmng = nullptr);

public slots:
    void setDay(const QDate &day);

signals:

private:
    std::shared_ptr<TimetableManager> ttmng;
    std::optional<QDate> currDay;
    float minToPixRatio = 5.f;    // minute/px
    int topPadding = 6;    // px
    int bottomPadding = 6;

    QGridLayout *mainLayout = nullptr;

    void buildWidget();    // call this only if startTime or finishTime has changed

    QTime startTime {11, 00};
    QTime finishTime {20, 00};

};

#endif // DAYTIMETABLEINFO_H
