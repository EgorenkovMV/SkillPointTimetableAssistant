#ifndef STUDENT_H
#define STUDENT_H

#include <memory>
#include <vector>
#include <QString>
#include <QMap>
#include <QDateTime>
#include <QJsonObject>


typedef int Ruble;
typedef int Minute;

class Lesson;

class Student
{
public:
    QString name;    // name is the key and have to be unique
    Ruble grossPayments;
    QMap<QDateTime, Ruble> paymentsHistory;
    QString educationalPlan;
    QString educationalPlanProgress;

    Student(const QString &name);
    void addPayment(Ruble payment, const QDateTime &date = QDateTime::currentDateTime());
    QJsonObject toJson() const;
    static Student fromJson(const QJsonObject &json);


};

#endif // STUDENT_H
