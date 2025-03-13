#include "studentslist.h"
#include "ui_studentslist.h"
#include <QVBoxLayout>
#include <QLabel>

StudentsList::StudentsList(QWidget *parent, std::shared_ptr<TimetableManager> ttmng)
    : QWidget(parent)
    , ttmng(ttmng)
    , ui(new Ui::StudentsList)
{
    ui->setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QMap<QString, std::shared_ptr<Student>>::const_iterator it = ttmng->students.constBegin();
    while (it != ttmng->students.constEnd()) {
        QLabel *label = new QLabel(it.value()->name);
        layout->addWidget(label);
        ++it;
    }
}

StudentsList::~StudentsList()
{
    delete ui;
}
