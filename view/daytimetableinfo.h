#ifndef DAYTIMETABLEINFO_H
#define DAYTIMETABLEINFO_H

#include <QWidget>
#include <QFrame>
#include <QDate>
#include <QTime>
#include <QLayout>
#include <QPushButton>
#include "../model/timetablemanager.h"
#include "lessoneditpopup.h"


class LessonTile : public QFrame
{
    Q_OBJECT
public:
    explicit LessonTile(QWidget *parent = nullptr, const std::shared_ptr<Lesson> &lesson = nullptr);

signals:
    void lessonClicked(const std::shared_ptr<Lesson> &lesson);

private:
    std::shared_ptr<Lesson> lesson;
    static QVector<QString> colorCycleNames;
    static int colorCycleCounter;

    void mouseDoubleClickEvent(QMouseEvent* event) override;

};

class DayTimetableInfo : public QWidget
{
    Q_OBJECT
public:
    explicit DayTimetableInfo(QWidget *parent = nullptr, const std::shared_ptr<TimetableManager> &ttmng = nullptr);
    ~DayTimetableInfo();

public slots:
    void setDay(const QDate &day);
    void editLesson(const std::shared_ptr<Lesson> &lesson);
    void createLesson();
    void deleteLessonEdit();

signals:

private:
    std::shared_ptr<TimetableManager> ttmng;
    std::optional<QDate> currDay;
    float minToPixRatio = 5.f;    // minute/px
    int topPadding = 6;    // px
    int bottomPadding = 6;

    QGridLayout *mainLayout = nullptr;

    LessonEditPopup *lessonEdit = nullptr;


    void buildWidget();    // call this only if startTime or finishTime has changed
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void setupLessonPopup();


};

#endif // DAYTIMETABLEINFO_H
