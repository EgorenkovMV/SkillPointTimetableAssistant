#include "lessoneditpopup.h"
#include "ui_lessoneditpopup.h"

LessonEditPopup::LessonEditPopup(QWidget *parent,
                                 const std::shared_ptr<TimetableManager> &ttmng,
                                 const std::shared_ptr<Lesson> &lesson,
                                 const QDate &date)
    : QWidget(parent)
    , ui(new Ui::LessonEditPopup)
    , ttmng(ttmng)
    , lesson(lesson)
{
    ui->setupUi(this);

    task = lesson ? PopupTask::Edit : PopupTask::Create;

    connect(ui->pb_accept, &QPushButton::clicked, this, &LessonEditPopup::saveLesson);
    connect(ui->pb_cancel, &QPushButton::clicked, this, &LessonEditPopup::canceled);

    if (lesson) {
        ui->label->setText(lesson->participants.at(0)->student->name);
    }

    if (!ttmng) {
        qWarning("LessonEditPopup::LessonEditPopup: no ttmng");
        emit canceled();
        return;
    }

}

LessonEditPopup::~LessonEditPopup()
{
    delete ui;
    qDebug() << "~LessonEditPopup()";
}

void LessonEditPopup::saveLesson()
{
    if (task == PopupTask::Edit) {
        emit lessonEdited(lesson);
    }
    else {
        emit lessonCreated(lesson);
    }
}

void LessonEditPopup::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit canceled();
}


