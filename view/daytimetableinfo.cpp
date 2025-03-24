#include "daytimetableinfo.h"
#include <QLayoutItem>
#include <QLabel>
#include <QMouseEvent>

namespace {

void clearLayout(QLayout *layout) {
    if (layout == NULL)
        return;
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}
} // namespace


QVector<QString> LessonTile::colorCycleNames = {"blue", "red", "green"};
int LessonTile::colorCycleCounter = 0;


LessonTile::LessonTile(QWidget *parent, const std::shared_ptr<Lesson> &lesson)
    : QFrame{parent}
    , lesson(lesson)
{


    setFrameStyle(QFrame::StyledPanel);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (!lesson) {
        setStyleSheet("LessonTile {\
                       border-color: rgba(0, 0, 0, 100%);\
                       border-style: solid;\
                       border-width: 1px; }");
        return;
    }


    QString color = colorCycleNames.at(colorCycleCounter % colorCycleNames.size());
    ++colorCycleCounter;
    setStyleSheet(QString("LessonTile {\
                           border-color: %1;\
                           border-style: solid;\
                           border-width: 2px; }").arg(color));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QString lessonName = "???";
    if (!lesson->participants.empty()) {
        lessonName = lesson->participants.at(0)->student->name;
        if (lesson->participants.size() > 1) {
            lessonName.append(" и др.");
        }
    }
    layout->addWidget(new QLabel(lessonName));
}

void LessonTile::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (!lesson) {
        event->ignore();
        return;
    }
    emit clicked(lesson);
    event->accept();
}



DayTimetableInfo::DayTimetableInfo(QWidget *parent, const std::shared_ptr<TimetableManager> &ttmng)
    : QWidget{parent}
    , ttmng(ttmng)
{
    buildWidget();
}

void DayTimetableInfo::buildWidget()
{
    if (startTime > finishTime) {
        std::swap(startTime, finishTime);
    }


    clearLayout(layout());

    mainLayout = new QGridLayout(this);
    mainLayout->setVerticalSpacing(0);
    mainLayout->setContentsMargins(QMargins {11, 0, 11, 0});

    int rowCounter = 0;
    for (int hour = startTime.hour(); hour < finishTime.hour(); ++hour) {
        QFrame *line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Plain);
        mainLayout->addWidget(line, rowCounter, 0, 1, -1);
        mainLayout->addWidget(new QLabel(QString::number(hour) + ":00"), rowCounter + 1, 0, 1, 1);    // Qt::AlignHCenter | Qt::AlignVCenter

        rowCounter += 12;
    }


    if (currDay) {
        setDay(currDay.value());
    }
}


void DayTimetableInfo::setDay(const QDate &day)
{
    currDay = day;

    if (!mainLayout) {
        buildWidget();
    }


    // Clear the column #1 (lesson tiles)
    for (int row = 0; row < mainLayout->rowCount(); ++row) {
        QLayoutItem *item = mainLayout->itemAtPosition(row, 1);
        if (!item) {
            continue;
        }
        if (dynamic_cast<LessonTile *>(item->widget())) {
            delete item->widget();
        }
        else if (dynamic_cast<QLabel *>(item->widget())) {
            delete item->widget();
        }
    }

    // Adding lesson tiles
    for (const std::shared_ptr<Lesson> &lesson : ttmng->lessons) {
        if (lesson->date.date() == currDay) {
            int offset = (float)((lesson->date.time().hour() - startTime.hour()) * 60 + lesson->date.time().minute()) / minToPixRatio;
            if (offset < 0) {
                qWarning("DayTimetableInfo::setDay: lesson is outside of working hours");
                continue;
            }
            int duration = (float)lesson->duration / minToPixRatio;
            LessonTile *tile = new LessonTile(nullptr, lesson);
            connect(tile, &LessonTile::clicked, this, &DayTimetableInfo::editLesson);
            mainLayout->addWidget(tile, offset, 1, duration, 1);
        }
    }

    // Adding blank lesson tiles
    int spacerStart = -1;
    int spacerEnd = -1;
    int row = 0;
    for (row = 0; row < (finishTime.hour() - startTime.hour()) * 60 / minToPixRatio; ++row) {
        if (!mainLayout->itemAtPosition(row, 1)) {
            if (spacerStart == -1) {
                spacerStart = row;
            }
            spacerEnd = row;
        }
        else {
            if (spacerStart != -1) {
                mainLayout->addWidget(new LessonTile(nullptr, nullptr), spacerStart, 1, spacerEnd - spacerStart + 1, 1);
            }
            spacerStart = -1;
            spacerEnd = -1;
        }
    }
    if (!mainLayout->itemAtPosition(row, 1)) {
        if (spacerStart != -1) {
            mainLayout->addWidget(new LessonTile(nullptr, nullptr), spacerStart, 1, spacerEnd - spacerStart + 1, 1);
        }
    }

}

void DayTimetableInfo::editLesson(const std::shared_ptr<Lesson> &lesson)
{
    if (!lesson) {
        qWarning("DayTimetableInfo::editLesson: no lesson to edit");
        return;
    }

    if (lessonEdit) {
        lessonEdit->show();
        lessonEdit->setFocus();
        return;
    }

    lessonEdit = new LessonEditPopup(nullptr, ttmng, lesson);
    connect(lessonEdit, &LessonEditPopup::lessonEdited, this, &DayTimetableInfo::deleteLessonEdit);
    connect(lessonEdit, &LessonEditPopup::lessonCreated, this, &DayTimetableInfo::deleteLessonEdit);
    connect(lessonEdit, &LessonEditPopup::canceled, this, &DayTimetableInfo::deleteLessonEdit);

}

void DayTimetableInfo::createLesson()
{
    if (lessonEdit) {
        lessonEdit->show();
        lessonEdit->setFocus();
        return;
    }

    lessonEdit = new LessonEditPopup(nullptr, ttmng, nullptr, currDay.value_or(QDate::currentDate()));
    connect(lessonEdit, &LessonEditPopup::lessonEdited, this, &DayTimetableInfo::deleteLessonEdit);
    connect(lessonEdit, &LessonEditPopup::lessonCreated, this, &DayTimetableInfo::deleteLessonEdit);
    connect(lessonEdit, &LessonEditPopup::canceled, this, &DayTimetableInfo::deleteLessonEdit);
}

void DayTimetableInfo::deleteLessonEdit()
{
    if (currDay) {
        setDay(currDay.value());
    }

    lessonEdit->hide();
    lessonEdit->deleteLater();
    lessonEdit = nullptr;
}

void DayTimetableInfo::mouseDoubleClickEvent(QMouseEvent* event)
{
    createLesson();
    event->accept();
}



