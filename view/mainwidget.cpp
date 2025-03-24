#include "mainwidget.h"
#include "./ui_mainwidget.h"
#include "studentslist.h"
#include "timetablewidget.h"


MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ttmng = std::shared_ptr<TimetableManager> {new TimetableManager {}};
    ttmng->load();

    ui->tabWidget->addTab(new TimetableWidget(this, ttmng), "Расписание");
    ui->tabWidget->addTab(new StudentsList(this, ttmng), "Ученики");
    setWindowTitle("SkillPoint Timetable Assistant");
}

MainWidget::~MainWidget()
{
    delete ui;
}
