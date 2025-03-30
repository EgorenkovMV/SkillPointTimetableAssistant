#include "lessoneditpopup.h"
#include "ui_lessoneditpopup.h"
#include <QListWidgetItem>

LessonEditPopup::LessonEditPopup(QWidget *parent,
                                 const std::shared_ptr<TimetableManager> &ttmng,
                                 const std::shared_ptr<Lesson> &lesson,
                                 const QDate &date)
    : QWidget(parent)
    , ui(new Ui::LessonEditPopup)
    , ttmng(ttmng)
    , lesson(lesson)
    , date(date)
{
    ui->setupUi(this);

    if (!ttmng) {
        qWarning("LessonEditPopup::LessonEditPopup: no ttmng");
        emit canceled();
        return;
    }

    task = lesson ? PopupTask::Edit : PopupTask::Create;

    connect(ui->pb_accept, &QPushButton::clicked, this, &LessonEditPopup::saveLesson);
    connect(ui->pb_discard, &QPushButton::clicked, this, &LessonEditPopup::canceled);
    connect(ui->sb_cost, &QSpinBox::valueChanged, this, [this] (int value) {
        this->ui->sb_partiCost->setValue(value);
    });

    QMap<QString, std::shared_ptr<Student>>::const_iterator it = ttmng->students.constBegin();
    while (it != ttmng->students.constEnd()) {
        ui->lw_students->addItem(it.key());
        ++it;
    }
    if (ttmng->students.size() > 0) {
        ui->lw_students->setCurrentRow(0);
    }


    if (task == PopupTask::Create) {
        this->lesson = std::shared_ptr<Lesson>(new Lesson());
    }




}

LessonEditPopup::~LessonEditPopup()
{
    delete ui;
}

void LessonEditPopup::saveLesson()
{
    lesson->cost = ui->sb_cost->value();
    lesson->date = QDateTime(date, ui->te_time->time());
    lesson->duration = ui->sb_duration->value();



    if (task == PopupTask::Edit) {
        emit lessonEdited(lesson);
    }
    else {
        ttmng->lessons.push_back(lesson);
        emit lessonCreated(lesson);
    }
}

void LessonEditPopup::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit canceled();
}


