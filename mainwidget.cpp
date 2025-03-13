#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ttmng.load();
    QString loadReport {"Lessons:\n"};
    for (const std::shared_ptr<Lesson> &lesson : ttmng.lessons) {
        loadReport.append(lesson->participants[0]->student->name + '\n');
        loadReport.append(QString::number(lesson->cost) + '\n');
    }

    ui->pte_students->setPlainText(loadReport);
}

MainWidget::~MainWidget()
{
    delete ui;
}
