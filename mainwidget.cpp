#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ttmng.load();
    QString loadReport {"Students:\n"};
    for (const Student &student : ttmng.students) {
        loadReport.append(student.name + '\n');
        loadReport.append(QString::number(student.grossPayments) + '\n');
    }

    ui->pte_students->setPlainText(loadReport);
}

MainWidget::~MainWidget()
{
    delete ui;
}
