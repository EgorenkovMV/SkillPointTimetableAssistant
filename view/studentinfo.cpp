#include "studentinfo.h"
#include "ui_studentinfo.h"

StudentInfo::StudentInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudentInfo)
{
    ui->setupUi(this);

    saveDialog = new QMessageBox(this);
    saveDialog->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    saveDialog->setText("Сохранить изменения?");
    saveDialog->setWindowTitle("Сохранить изменения?");
    saveDialog->setModal(true);
    saveDialog->done(-1);

    discardDialog = new QMessageBox(this);
    discardDialog->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    discardDialog->setText("Отменить изменение данных этого ученика?");
    discardDialog->setWindowTitle("Отменить изменение?");
    discardDialog->setModal(true);
    discardDialog->done(-1);

    paymentDialog = new QDialog(this);
    paymentDialog->setModal(true);
    paymentDialog->done(-1);


    connect(ui->pb_editStudent, &QPushButton::clicked, this, &StudentInfo::startEditing);
    connect(ui->pb_save, &QPushButton::clicked, this, &StudentInfo::saveClicked);
    connect(ui->pb_discard, &QPushButton::clicked, this, &StudentInfo::discardClicked);
    connect(ui->pb_addPayment, &QPushButton::clicked, this, &StudentInfo::addPaymentClicked);

    connect(saveDialog, &QDialog::accepted, this, &StudentInfo::stopEditingAndSave);
    connect(discardDialog, &QDialog::accepted, this, &StudentInfo::stopEditingAndDiscard);

    // connect(ui->pb_addPayment, &QPushButton::clicked, this, &StudentInfo::addPayment);

    stopEditingAndDiscard();
}

StudentInfo::~StudentInfo()
{
    delete ui;
}

void StudentInfo::setStudent(const std::shared_ptr<Student> &student)
{
    if (!student) {
        return;
    }

    currStudent = student;

    ui->le_name->setText(currStudent->name);
    ui->le_educationalPlan->setText(currStudent->educationalPlan);
    ui->le_educationalPlanProgress->setText(currStudent->educationalPlanProgress);
    ui->sb_grossPayment->setValue(currStudent->grossPayments);

    ui->tw_paymentHistory->clear();
    ui->tw_paymentHistory->setRowCount(currStudent->paymentsHistory.size());
    ui->tw_paymentHistory->setColumnCount(2);

    QMap<QDateTime, Ruble>::const_iterator it = currStudent->paymentsHistory.constBegin();
    int rowCount = 0;
    while (it != currStudent->paymentsHistory.constEnd()) {
        QTableWidgetItem *dateItem = new QTableWidgetItem(it.key().toString());
        QTableWidgetItem *paymentItem = new QTableWidgetItem(QString::number(it.value()));
        ui->tw_paymentHistory->setItem(rowCount, 0, dateItem);
        ui->tw_paymentHistory->setItem(rowCount, 1, paymentItem);
        ++it;
        ++rowCount;
    }
    ui->tw_paymentHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_paymentHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void StudentInfo::startEditing()
{
    ui->le_name->setReadOnly(false);
    ui->le_educationalPlan->setReadOnly(false);
    ui->le_educationalPlanProgress->setReadOnly(false);
    ui->sb_grossPayment->setReadOnly(false);

    ui->pb_editStudent->setDisabled(true);
    ui->pb_save->setDisabled(false);
    ui->pb_discard->setDisabled(false);
    ui->pb_addPayment->setDisabled(true);

}

void StudentInfo::saveClicked()
{
    if (ui->le_name->text().isEmpty()) {
        saveDialog->setText("Ученика без имени сохранить не получится. Продолжить?");
    }
    else if (!currStudent) {
        saveDialog->setText("Создать и сохранить нового ученика?");
    }
    else if (ui->le_name->text() == currStudent->name) {
        saveDialog->setText("Сохранить изменения для этого ученика?");
    }
    else {
        saveDialog->setText("Создать и сохранить нового ученика?");
    }
    saveDialog->open();
}

void StudentInfo::discardClicked()
{
    discardDialog->open();
}

void StudentInfo::addPaymentClicked()
{

}

void StudentInfo::stopEditingAndSave()
{
    ui->le_name->setReadOnly(true);
    ui->le_educationalPlan->setReadOnly(true);
    ui->le_educationalPlanProgress->setReadOnly(true);
    ui->sb_grossPayment->setReadOnly(true);

    ui->pb_editStudent->setDisabled(false);
    ui->pb_save->setDisabled(true);
    ui->pb_discard->setDisabled(true);
    ui->pb_addPayment->setDisabled(false);

    if (ui->le_name->text().isEmpty()) {
        qWarning("StudentInfo::stopEditingAndSave: trying to save student without a name");
        return;
    }

    if (currStudent) {
        if (ui->le_name->text() == currStudent->name) {
            currStudent->educationalPlan = ui->le_educationalPlan->text();
            currStudent->educationalPlanProgress = ui->le_educationalPlanProgress->text();
            return;
        }
    }

    Student *newStudent = new Student{ui->le_name->text()};
    newStudent->addPayment(ui->sb_grossPayment->value());
    newStudent->educationalPlan = ui->le_educationalPlan->text();
    newStudent->educationalPlanProgress = ui->le_educationalPlanProgress->text();

    setStudent(std::shared_ptr<Student>(newStudent));
    emit studentCreated(currStudent);

}

void StudentInfo::stopEditingAndDiscard()
{
    ui->le_name->setReadOnly(true);
    ui->le_educationalPlan->setReadOnly(true);
    ui->le_educationalPlanProgress->setReadOnly(true);
    ui->sb_grossPayment->setReadOnly(true);

    ui->pb_editStudent->setDisabled(false);
    ui->pb_save->setDisabled(true);
    ui->pb_discard->setDisabled(true);
    ui->pb_addPayment->setDisabled(false);

    if (currStudent) {
        setStudent(currStudent);
    }
}

void StudentInfo::addPayment()
{
    currStudent->addPayment(0);
}



