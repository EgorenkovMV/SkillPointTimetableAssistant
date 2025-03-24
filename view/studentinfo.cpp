#include "studentinfo.h"
#include "ui_studentinfo.h"
#include <QCalendarWidget>

StudentInfo::StudentInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudentInfo)
{
    ui->setupUi(this);

    saveDialog = new QMessageBox(this);
    saveDialog->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    saveDialog->setText("Сохранить изменения?");
    saveDialog->setWindowTitle("Сохранить изменения?");
    saveDialog->setIcon(QMessageBox::Question);
    saveDialog->setModal(true);

    discardDialog = new QMessageBox(this);
    discardDialog->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    discardDialog->setText("Отменить изменение данных этого ученика?");
    discardDialog->setWindowTitle("Отменить изменение?");
    discardDialog->setIcon(QMessageBox::Question);
    discardDialog->setModal(true);

    paymentDialog = new QMessageBox(this);
    paymentDialog->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    paymentDialog->setText("Введите сумму и дату");
    paymentDialog->setWindowTitle("Создайте запись о внесении оплаты");

    QGridLayout *dialogLayout = dynamic_cast<QGridLayout *>(paymentDialog->layout());

    paymentDialogDateEdit = new QDateTimeEdit();
    paymentDialogDateEdit->setCalendarPopup(true);
    paymentDialogDateEdit->calendarWidget()->setFirstDayOfWeek(Qt::DayOfWeek::Monday);
    paymentDialogDateEdit->calendarWidget()->setGridVisible(true);
    paymentDialogDateEdit->setDateTime(QDateTime::currentDateTime());

    paymentDialogSpinBox = new QSpinBox();
    paymentDialogSpinBox->setMaximum(999999);
    paymentDialogSpinBox->setSingleStep(50);
    paymentDialogSpinBox->setSuffix(" руб.");
    if (dialogLayout) {
        dialogLayout->addWidget(paymentDialogDateEdit, 1, 1);
        dialogLayout->addWidget(paymentDialogSpinBox, 1, 2);
    }
    else {
        paymentDialog->layout()->addWidget(paymentDialogDateEdit);
        paymentDialog->layout()->addWidget(paymentDialogSpinBox);
    }

    paymentDialog->setModal(true);

    connect(ui->pb_editStudent, &QPushButton::clicked, this, &StudentInfo::startEditing);
    connect(ui->pb_save, &QPushButton::clicked, this, &StudentInfo::saveClicked);
    connect(ui->pb_discard, &QPushButton::clicked, this, &StudentInfo::discardClicked);
    connect(ui->pb_addPayment, &QPushButton::clicked, this, &StudentInfo::addPaymentClicked);

    connect(saveDialog, &QDialog::accepted, this, &StudentInfo::stopEditingAndSave);
    connect(discardDialog, &QDialog::accepted, this, &StudentInfo::stopEditingAndDiscard);
    connect(paymentDialog, &QDialog::accepted, this, &StudentInfo::addPayment);

    ui->tw_contactData->setRowCount(1);
    ui->tw_contactData->setColumnCount(2);
    ui->tw_contactData->setHorizontalHeaderLabels({"Контактное лицо", "Номер телефона"});
    ui->tw_contactData->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_contactData->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->tw_contactData->model(), &QAbstractItemModel::dataChanged, this, [this] (const QModelIndex &index) {
        if (index.row() == ui->tw_contactData->rowCount() - 1) {
            ui->tw_contactData->insertRow(ui->tw_contactData->rowCount());
        }
    });

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
    ui->sb_grossPayment->setValue(currStudent->paymentBalance());

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


    ui->tw_contactData->clear();
    ui->tw_contactData->setRowCount(currStudent->contactData.size() + 1);
    ui->tw_contactData->setColumnCount(2);
    ui->tw_contactData->setHorizontalHeaderLabels({"Контактное лицо", "Номер телефона"});
    rowCount = 0;
    for (const ContactData &contact : currStudent->contactData) {
        QTableWidgetItem *contactName = new QTableWidgetItem(contact.name);
        QTableWidgetItem *contactPhone = new QTableWidgetItem(contact.phoneNumber);
        ui->tw_contactData->setItem(rowCount, 0, contactName);
        ui->tw_contactData->setItem(rowCount, 1, contactPhone);
        ++rowCount;
    }
    ui->tw_contactData->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_contactData->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

}

void StudentInfo::responseToStudentDeletion(const std::shared_ptr<Student> &student)
{
    if (student == currStudent) {
        setStudent(nullptr);
    }
}

void StudentInfo::startEditing()
{
    ui->le_name->setReadOnly(false);
    ui->le_educationalPlan->setReadOnly(false);
    ui->le_educationalPlanProgress->setReadOnly(false);
    ui->sb_grossPayment->setReadOnly(false);
    ui->tw_contactData->setEditTriggers(QAbstractItemView::AllEditTriggers);

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
    if (currStudent) {
        paymentDialog->open();
    }
}

void StudentInfo::stopEditingAndSave()
{
    ui->le_name->setReadOnly(true);
    ui->le_educationalPlan->setReadOnly(true);
    ui->le_educationalPlanProgress->setReadOnly(true);
    ui->sb_grossPayment->setReadOnly(true);
    ui->tw_contactData->setEditTriggers(QAbstractItemView::NoEditTriggers);

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
            if (ui->sb_grossPayment->value() - currStudent->paymentBalance()) {
                currStudent->addPayment(ui->sb_grossPayment->value() - currStudent->paymentBalance());
            }
            currStudent->educationalPlan = ui->le_educationalPlan->text();
            currStudent->educationalPlanProgress = ui->le_educationalPlanProgress->text();

            currStudent->contactData.clear();
            for (int row = 0; row < ui->tw_contactData->rowCount(); ++row) {
                QTableWidgetItem *nameItem = ui->tw_contactData->item(row, 0);
                QTableWidgetItem *phoneItem = ui->tw_contactData->item(row, 1);

                if (!nameItem or !phoneItem) continue;

                QString name = nameItem->data(Qt::EditRole).toString();   // DisplayRole / EditRole
                QString phone = phoneItem->data(Qt::EditRole).toString();
                if (!phone.isEmpty()) {
                    if (name.isEmpty()) {
                        name = currStudent->name;
                    }
                    currStudent->contactData.push_back({name, phone});
                }
            }

            setStudent(currStudent);
            return;
        }
    }

    Student *newStudent = new Student{ui->le_name->text()};
    newStudent->addPayment(ui->sb_grossPayment->value());
    newStudent->educationalPlan = ui->le_educationalPlan->text();
    newStudent->educationalPlanProgress = ui->le_educationalPlanProgress->text();

    for (int row = 0; row < ui->tw_contactData->rowCount(); ++row) {
        QTableWidgetItem *nameItem = ui->tw_contactData->item(row, 0);
        QTableWidgetItem *phoneItem = ui->tw_contactData->item(row, 1);

        if (!nameItem or !phoneItem) continue;

        QString name = nameItem->data(Qt::EditRole).toString();   // DisplayRole / EditRole
        QString phone = phoneItem->data(Qt::EditRole).toString();
        if (!phone.isEmpty()) {
            if (name.isEmpty()) {
                name = currStudent->name;
            }
            newStudent->contactData.push_back({name, phone});
        }
    }

    setStudent(std::shared_ptr<Student>(newStudent));
    emit studentCreated(currStudent);

}

void StudentInfo::stopEditingAndDiscard()
{
    ui->le_name->setReadOnly(true);
    ui->le_educationalPlan->setReadOnly(true);
    ui->le_educationalPlanProgress->setReadOnly(true);
    ui->sb_grossPayment->setReadOnly(true);
    ui->tw_contactData->setEditTriggers(QAbstractItemView::NoEditTriggers);

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
    if (!currStudent or paymentDialogSpinBox->value() == 0) {
        return;
    }
    currStudent->addPayment(paymentDialogSpinBox->value(), paymentDialogDateEdit->dateTime());

    setStudent(currStudent);
}



