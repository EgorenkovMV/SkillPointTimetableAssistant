#ifndef STUDENT_H
#define STUDENT_H

#include <memory>
#include <vector>
#include <QString>
#include <QMap>
#include "lesson.h"


class Student
{
public:
    QString name;
    std::vector<std::shared_ptr<Lesson>> lessons;
    Ruble grossPayments;
    QMap<QDateTime, Ruble> paymentsHistory;

    Student(const QString &name);
    Student(const QJsonObject &json);
    void addPayment(Ruble payment, const QDateTime &date = QDateTime::currentDateTime());
    void addLesson(const std::shared_ptr<Lesson> &lesson);
    Ruble getPaymentResidue() const;
    QJsonObject toJson() const;

};

#endif // STUDENT_H
