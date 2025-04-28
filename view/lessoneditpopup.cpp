#include "lessoneditpopup.h"
#include "ui_lessoneditpopup.h"
#include <QListWidgetItem>
#include "../constants.h"

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

    if (!ttmng) {
        qWarning("LessonEditPopup::LessonEditPopup: no ttmng");
        emit canceled();
        return;
    }

    task = lesson ? PopupTask::Edit : PopupTask::Create;
    this->date = lesson ? lesson->date.date() : date;

    connect(ui->pb_accept, &QPushButton::clicked, this, &LessonEditPopup::saveLesson);
    connect(ui->pb_discard, &QPushButton::clicked, this, &LessonEditPopup::canceled);
    connect(ui->pb_accept, &QPushButton::clicked, this, &LessonEditPopup::saveLesson);
    connect(ui->pb_discard, &QPushButton::clicked, this, &LessonEditPopup::canceled);
    connect(ui->sb_cost, &QSpinBox::valueChanged, this, [this] (int value) {
        this->ui->sb_partiCost->setValue(value);
    });

    connect(ui->pb_addParti, &QPushButton::clicked, this, &LessonEditPopup::addPartiButton);
    connect(ui->lw_parti, &PartiList::addedParti, this, &LessonEditPopup::addParti);
    connect(ui->pb_removeSelectedParti, &QPushButton::clicked, this, &LessonEditPopup::removeParti);


    ui->cb_type->addItem("EnglishIndividual");
    ui->cb_type->addItem("EnglishGroup");
    ui->cb_type->setCurrentIndex(0);

    ui->cb_partiOutcome->addItem("NotFinishedYet");
    ui->cb_partiOutcome->addItem("Successful");
    ui->cb_partiOutcome->addItem("CanceledNotRefunded");
    ui->cb_partiOutcome->addItem("CanceledRefunded");
    ui->cb_partiOutcome->setCurrentIndex(0);

    QMap<QString, std::shared_ptr<Student>>::const_iterator it = ttmng->students.constBegin();
    while (it != ttmng->students.constEnd()) {
        ui->lw_students->addItem(it.key());
        ++it;
    }

    ui->te_time->setMinimumTime(Constants::startTime);
    ui->te_time->setMaximumTime(Constants::finishTime);
    ui->te_time->setTime(Constants::startTime);


    if (task == PopupTask::Create) {
        this->lesson = std::shared_ptr<Lesson>(new Lesson());
    }
    else {
        participants = this->lesson->participants;
        ui->te_time->setTime(this->lesson->date.time());
        ui->sb_cost->setValue(this->lesson->cost);

        for (const std::shared_ptr<ParticipantInfo> &parti : this->lesson->participants) {
            ui->lw_parti->addItem(parti->student->name);
        }
    }
}

LessonEditPopup::~LessonEditPopup()
{
    delete ui;
}

void LessonEditPopup::saveLesson()
{
    lesson->date = QDateTime(date, ui->te_time->time());
    lesson->duration = ui->sb_duration->value();
    lesson->type = static_cast<Lesson::Type>(ui->cb_type->currentIndex());
    lesson->cost = ui->sb_cost->value();
    lesson->participants = participants;
    lesson->additionalInfo = ui->pte_additionalInfo->toPlainText();

    if (task == PopupTask::Edit) {
        if (lesson->participants.empty()) {
            ttmng->lessons.erase(std::find(ttmng->lessons.begin(), ttmng->lessons.end(), lesson));
            emit lessonDeleted(lesson);
        }
        else {
            emit lessonEdited(lesson);
        }
    }
    else {
        ttmng->lessons.push_back(lesson);
        emit lessonCreated(lesson);
    }
}

void LessonEditPopup::addParti(const QStringList &parti)
{
    for (const QString &itemName : parti) {
        std::shared_ptr<Student> student = *ttmng->students.find(itemName);
        if (!student) {
            qWarning() << "LessonEditPopup::addParti: no student in ttmng";
            continue;
        }
        ParticipantInfo parti {student, ui->sb_partiCost->value(),
                              static_cast<ParticipantInfo::Outcome>(ui->cb_partiOutcome->currentIndex())};

        participants.push_back(std::make_shared<ParticipantInfo>(parti));
        ui->lw_parti->findItems(itemName,
            Qt::MatchExactly).at(0)->setToolTip(QString::number(ui->sb_partiCost->value()));
    }
}

void LessonEditPopup::addPartiButton()
{
    QStringList newParti;
    for (QListWidgetItem* item : ui->lw_students->selectedItems()) {
        newParti.append(item->text());
        ui->lw_parti->addItem(item->text());
        ui->lw_students->takeItem(ui->lw_students->row(item));
    }
    addParti(newParti);
}

void LessonEditPopup::removeParti()
{
    if (ui->lw_parti->selectedItems().isEmpty()) {
        return;
    }
    participants.erase(std::find_if(participants.begin(), participants.end(), [this] (const std::shared_ptr<ParticipantInfo> &parti) {
        return parti->student->name == this->ui->lw_parti->selectedItems().at(0)->text();
    }));
    ui->lw_students->addItems({ui->lw_parti->selectedItems().at(0)->text()});
    delete ui->lw_parti->selectedItems().at(0);
}

void LessonEditPopup::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit canceled();
}


