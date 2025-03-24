#ifndef STUDENTSLIST_H
#define STUDENTSLIST_H

#include <QWidget>
#include <QFrame>
#include <QPushButton>
#include <QMessageBox>
#include "../model/timetablemanager.h"
#include "studentinfo.h"


class StudentsListItem : public QFrame
{
    Q_OBJECT

public:
    explicit StudentsListItem(QWidget *parent = nullptr, const std::shared_ptr<Student> &student = nullptr);
    void select();
    void deselect();

    std::shared_ptr<Student> student;

signals:
    void selected(StudentsListItem *item);
    void archived(const std::shared_ptr<Student> &student);

private slots:
    void removeButtonClicked();
    void removalConfirmed();

private:
    QPushButton *pb_removeStudent;
    bool isSelected = false;
    QMessageBox *removeConfirmDialog = nullptr;

    void mousePressEvent(QMouseEvent* event) override;

};

class StudentsList : public QWidget
{
    Q_OBJECT

public:
    explicit StudentsList(QWidget *parent = nullptr, std::shared_ptr<TimetableManager> ttmng = nullptr);
    ~StudentsList();
    void updateList();

private:
    std::shared_ptr<TimetableManager> ttmng;
    QWidget *wiInsideScrollArea;
    std::vector<StudentsListItem *> itemWidgets;
    StudentInfo *si_studentInfo;

    void handleSelect(StudentsListItem *item);
};

#endif // STUDENTSLIST_H
