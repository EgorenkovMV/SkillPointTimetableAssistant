#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ttmng = std::shared_ptr<TimetableManager> {new TimetableManager {}};
    ttmng->load();

    ui->tabWidget->addTab(new QWidget(this), tr("Расписание"));    // Not implemented yet
    ui->tabWidget->addTab(new StudentsList(this, ttmng), tr("Ученики"));
    ui->tabWidget->setCurrentIndex(1);
}

MainWidget::~MainWidget()
{
    delete ui;
}
