#ifndef STUDENT_H
#define STUDENT_H

#include <QString>
#include <QMap>
#include <QDateTime>
#include <QJsonObject>


typedef int Ruble;
typedef int Minute;

struct ContactData
{
    QString name;
    QString phoneNumber;

    QJsonObject toJson() const;
    static ContactData fromJson(const QJsonObject &json);

};

class Lesson;

class Student
{
public:
    QString name;    // name is the key and have to be unique
    QMap<QDateTime, Ruble> paymentsHistory;
    QString educationalPlan;
    QString educationalPlanProgress;
    QVector<ContactData> contactData;
    bool isArchived = false;

    Student(const QString &name);
    Ruble paymentBalance() const;
    void addPayment(Ruble payment, const QDateTime &date = QDateTime::currentDateTime());
    QJsonObject toJson() const;
    static Student fromJson(const QJsonObject &json);

private:
    Ruble cachedBalance;

};

#endif // STUDENT_H
