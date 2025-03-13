#ifndef STUDENTSLIST_H
#define STUDENTSLIST_H

#include <QWidget>
#include "../model/timetablemanager.h"

namespace Ui {
class StudentsList;
}

class StudentsList : public QWidget
{
    Q_OBJECT

public:
    explicit StudentsList(QWidget *parent = nullptr, std::shared_ptr<TimetableManager> ttmng = nullptr);
    ~StudentsList();

private:
    Ui::StudentsList *ui;
    std::shared_ptr<TimetableManager> ttmng;
};

#endif // STUDENTSLIST_H
