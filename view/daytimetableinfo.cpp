#include "daytimetableinfo.h"
#include <QLayoutItem>
#include <QLabel>

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
    QString color = colorCycleNames.at(colorCycleCounter % colorCycleNames.size());
    ++colorCycleCounter;
    setStyleSheet(QString("LessonTile {\
                           border-color: %1;\
                           border-style: solid;\
                           border-width: 2px; }").arg(color));

    QVBoxLayout *layout = new QVBoxLayout(this);
    QString lessonName = "???";
    if (lesson) {
        if (!lesson->participants.empty()) {
            lessonName = lesson->participants.at(0)->student->name;
            if (lesson->participants.size() > 1) {
                lessonName.append(" и др.");
            }
        }
    }
    layout->addWidget(new QLabel(lessonName));
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

    // setMinimumHeight((finishTime.hour() - startTime.hour()) * 60 / minToPixRatio +
    //      topPadding + bottomPadding + (finishTime.hour() - startTime.hour()) * 11);


    clearLayout(layout());

    mainLayout = new QGridLayout(this);
    mainLayout->setVerticalSpacing(0);
    mainLayout->setContentsMargins(QMargins {11, 1, 11, 0});

    //mainLayout->addWidget(widget, int fromRow, int fromColumn, int rowSpan, int columnSpan, Qt::Alignment alignment = Qt::Alignment());

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


    for (int row = 0; row <= (finishTime.hour() - startTime.hour()) * 60 / minToPixRatio; ++row) {
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


    for (const std::shared_ptr<Lesson> &lesson : ttmng->lessons) {
        if (lesson->date.date() == currDay) {
            int offset = (float)((lesson->date.time().hour() - startTime.hour()) * 60 + lesson->date.time().minute()) / minToPixRatio;
            int duration = (float)lesson->duration / minToPixRatio;
            mainLayout->addWidget(new LessonTile(nullptr, lesson), offset, 1, duration, 1);
        }
    }

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
                mainLayout->addWidget(new QLabel(""), spacerStart, 1, spacerEnd - spacerStart + 1, 1);
            }
            spacerStart = -1;
            spacerEnd = -1;
        }
    }
    if (!mainLayout->itemAtPosition(row, 1)) {
        if (spacerStart == -1) {
            mainLayout->addWidget(new QLabel(""), spacerStart, 1, spacerEnd - spacerStart + 1, 1);
        }
    }


    // mainLayout->addWidget(new LessonTile(nullptr, ttmng->lessons.at(0)), 0, 1, 20, 1);
    // mainLayout->addWidget(new QLabel(""), 8, 1, 6, 1);
    // mainLayout->addWidget(new LessonTile(nullptr, ttmng->lessons.at(1)), 20, 1, 12, 1);

}




