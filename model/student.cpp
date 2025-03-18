#include "student.h"
#include <QJsonArray>


Student::Student(const QString &name)
    : name(name)
    , grossPayments(0)
{}

void Student::addPayment(Ruble payment, const QDateTime &date)
{
    grossPayments += payment;
    paymentsHistory.insert(date, payment);
}

QJsonObject Student::toJson() const
{
    QJsonObject json;
    json["name"] = name;
    json["grossPayments"] = grossPayments;
    json["educationalPlan"] = educationalPlan;
    json["educationalPlanProgress"] = educationalPlanProgress;
    json["isArchived"] = isArchived;

    QJsonArray jsonPayments;
    QMap<QDateTime, Ruble>::const_iterator it = paymentsHistory.constBegin();
    while (it != paymentsHistory.constEnd()) {
        QJsonObject jsonPair;
        jsonPair["date"] = it.key().toString();
        jsonPair["payment"] = it.value();
        jsonPayments.append(jsonPair);
        ++it;
    }

    json["paymentsHistory"] = jsonPayments;

    return json;
}

Student Student::fromJson(const QJsonObject &json)
{
    Student student {""};
    student.name = json.value("name").toString();
    student.grossPayments = json.value("grossPayments").toInt();
    student.educationalPlan = json.value("educationalPlan").toString();
    student.educationalPlanProgress = json.value("educationalPlanProgress").toString();
    student.isArchived = json.value("isArchived").toBool(false);

    for (const QJsonValue &paymentRecord : json.value("paymentsHistory").toArray()) {
        student.paymentsHistory.insert(QDateTime::fromString(paymentRecord.toObject().value("date").toString()),
                                       paymentRecord.toObject().value("payment").toInt());
    }
    return student;
}


