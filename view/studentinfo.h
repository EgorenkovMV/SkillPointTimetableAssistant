#ifndef STUDENTINFO_H
#define STUDENTINFO_H

#include <QWidget>
#include <QMessageBox>
#include "../model/student.h"


namespace Ui {
class StudentInfo;
}

class StudentInfo : public QWidget
{
    Q_OBJECT

public:
    explicit StudentInfo(QWidget *parent = nullptr);
    ~StudentInfo();
    void setStudent(const std::shared_ptr<Student> &student = nullptr);

signals:
    void studentCreated(const std::shared_ptr<Student> &student);
    void paymentAdded(const std::shared_ptr<Student> &student);

private slots:
    void startEditing();

    void saveClicked();
    void discardClicked();
    void addPaymentClicked();

    void stopEditingAndSave();
    void stopEditingAndDiscard();
    void addPayment();

private:
    Ui::StudentInfo *ui;
    std::shared_ptr<Student> currStudent = nullptr;
    bool isEditable = false;
    QMessageBox *saveDialog = nullptr;
    QMessageBox *discardDialog = nullptr;
    QDialog *paymentDialog = nullptr;


};

#endif // STUDENTINFO_H
