#include "studentslist.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QStyle>

namespace {
int maxSLILabelSize = 25;    // SLI = StudentsListItem

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


StudentsListItem::StudentsListItem(QWidget *parent, const std::shared_ptr<Student> &student)
{
    setMinimumSize(200, 80);
    setMaximumHeight(80);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    removeConfirmDialog = new QMessageBox(this);
    removeConfirmDialog->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    removeConfirmDialog->setText("Вы действительно хотите перенести ученика в архив?");
    removeConfirmDialog->setWindowTitle("Удалить?");
    removeConfirmDialog->setIcon(QMessageBox::Question);
    removeConfirmDialog->setModal(true);

    deselect();

    this->student = student;

    QGridLayout *layout = new QGridLayout(this);

    if (!student) {
        layout->addWidget(new QLabel(student->name + "Error: no student in StudentsListItem::StudentsListItem"), 0, 0);
        return;
    }
    layout->addWidget(new QLabel(student->name.first(
                          std::min(maxSLILabelSize, (int)student->name.size()))), 0, 0);
    layout->addWidget(new QLabel(student->educationalPlan.first(
                          std::min(maxSLILabelSize, (int)student->educationalPlan.size()))), 0, 1);
    layout->addWidget(new QLabel(student->educationalPlanProgress.first(
                          std::min(maxSLILabelSize, (int)student->educationalPlanProgress.size()))), 1, 1);
    pb_removeStudent = new QPushButton("Удалить ученика");
    connect(pb_removeStudent, &QPushButton::clicked, this, &StudentsListItem::removeButtonClicked);
    connect(removeConfirmDialog, &QDialog::accepted, this, &StudentsListItem::removalConfirmed);

    layout->addWidget(pb_removeStudent, 1, 0);

    setMouseTracking(true);
}

void StudentsListItem::select()
{
    isSelected = true;

    setStyleSheet("StudentsListItem {\
                   border-width: 2px;\
                   border-style: solid; \
                   border-color: rgba(28, 73, 255, 0.79); }");
}

void StudentsListItem::deselect()
{
    isSelected = false;

    setStyleSheet("StudentsListItem {\
                   border-width: 2px;\
                   border-style: solid; \
                   border-color: rgba(0, 0, 0, 25%); }");
}

void StudentsListItem::removeButtonClicked()
{
    removeConfirmDialog->open();
}

void StudentsListItem::removalConfirmed()
{
    student->isArchived = true;
    hide();
    emit archived(student);
}

void StudentsListItem::mousePressEvent(QMouseEvent* event)
{
    emit selected(this);
    // select() is called from senior widget
}



StudentsList::StudentsList(QWidget *parent, std::shared_ptr<TimetableManager> ttmng)
    : QWidget(parent)
    , ttmng(ttmng)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    // Students list
    QScrollArea *scrollArea = new QScrollArea();
    new QVBoxLayout(scrollArea);
    layout->addWidget(scrollArea);

    wiInsideScrollArea = new QWidget();
    new QVBoxLayout(wiInsideScrollArea);

    scrollArea->setWidget(wiInsideScrollArea);
    scrollArea->setWidgetResizable(true);

    // Student info
    si_studentInfo = new StudentInfo();
    layout->addWidget(si_studentInfo);

    layout->setStretch(0, 1);
    layout->setStretch(1, 1);

    connect(si_studentInfo, &StudentInfo::studentCreated, this, [this] (const std::shared_ptr<Student> &student) {
        this->ttmng->addStudent(student);
        updateList();
    });

    updateList();
}

StudentsList::~StudentsList()
{
    ttmng->save();
}

void StudentsList::updateList()
{
    clearLayout(wiInsideScrollArea->layout());
    itemWidgets.clear();

    QMap<QString, std::shared_ptr<Student>>::const_iterator it = ttmng->students.constBegin();
    while (it != ttmng->students.constEnd()) {
        if (!it.value()->isArchived) {
            StudentsListItem *item = new StudentsListItem(nullptr, it.value());
            itemWidgets.push_back(item);
            wiInsideScrollArea->layout()->addWidget(item);
            connect(item, &StudentsListItem::selected, this, &StudentsList::handleSelect);
            connect(item, &StudentsListItem::archived, si_studentInfo, &StudentInfo::responseToStudentDeletion);
        }
        ++it;
    }
    QSpacerItem *spacer = new QSpacerItem(30, 30, QSizePolicy::Expanding, QSizePolicy::Expanding);
    dynamic_cast<QVBoxLayout *>(wiInsideScrollArea->layout())->addSpacerItem(spacer);
}

void StudentsList::handleSelect(StudentsListItem *item)
{
    for (StudentsListItem *itemWidget : itemWidgets) {
        if (itemWidget) {
            itemWidget->deselect();
        }
    }
    item->select();

    si_studentInfo->setStudent(item->student);
};
