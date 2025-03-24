#include "student.h"
#include <QJsonArray>


QJsonObject ContactData::toJson() const
{
    QJsonObject json;
    json["name"] = name;
    json["phoneNumber"] = phoneNumber;

    return json;
}

ContactData ContactData::fromJson(const QJsonObject &json)
{
    ContactData contactData {""};
    contactData.name = json.value("name").toString();
    contactData.phoneNumber = json.value("phoneNumber").toString();

    return contactData;
}



Student::Student(const QString &name)
    : name(name)
    , cachedBalance(0)
{}

Ruble Student::paymentBalance() const
{
    return cachedBalance;
}

void Student::addPayment(Ruble payment, const QDateTime &date)
{
    cachedBalance += payment;
    paymentsHistory.insert(date, payment);
}

QJsonObject Student::toJson() const
{
    QJsonObject json;
    json["name"] = name;
    json["cachedBalance"] = cachedBalance;
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

    QJsonArray jsonContacts;
    for (const ContactData &contact : contactData) {
        jsonContacts.append(contact.toJson());
    }
    json["contactData"] = jsonContacts;


    return json;
}

Student Student::fromJson(const QJsonObject &json)
{
    Student student {""};
    student.name = json.value("name").toString();
    student.cachedBalance = json.value("cachedBalance").toInt();
    student.educationalPlan = json.value("educationalPlan").toString();
    student.educationalPlanProgress = json.value("educationalPlanProgress").toString();
    student.isArchived = json.value("isArchived").toBool(false);

    for (const QJsonValue &paymentRecord : json.value("paymentsHistory").toArray()) {
        student.paymentsHistory.insert(QDateTime::fromString(paymentRecord.toObject().value("date").toString()),
                                       paymentRecord.toObject().value("payment").toInt());
    }

    for (const QJsonValue &contacRecord : json.value("contactData").toArray()) {
        student.contactData.push_back(ContactData::fromJson(contacRecord.toObject()));
    }

    return student;
}


