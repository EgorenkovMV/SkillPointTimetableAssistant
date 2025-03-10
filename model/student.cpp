#include "student.h"
#include <QJsonArray>


Student::Student(const QString &name)
    : name(name)
    , grossPayments(0)
{}

Student::Student(const QJsonObject &json)
{
    name = json.value("name").toString();
    for (const QJsonValue & lesson : json.value("lessons").toArray()) {
        lessons.push_back(std::shared_ptr<Lesson> (new Lesson (lesson.toObject(), this)));
    }
    grossPayments = json.value("grossPayments").toInt();
    for (const QJsonValue & paymentRecord : json.value("paymentsHistory").toArray()) {
        paymentsHistory.insert(QDateTime::fromString(paymentRecord.toObject().value("date").toString()),
                               paymentRecord.toObject().value("payment").toInt());
    }
}

void Student::addPayment(Ruble payment, const QDateTime &date)
{
    grossPayments += payment;
    paymentsHistory.insert(date, payment);
}

void Student::addLesson(const std::shared_ptr<Lesson> &lesson)
{
    lessons.push_back(lesson);
    lesson->student = this;
}

Ruble Student::getPaymentResidue() const
{
    Ruble residue = grossPayments;
    for (std::shared_ptr<Lesson> lesson : lessons) {
        if (lesson->outcome == Lesson::Outcome::Successful or lesson->outcome == Lesson::Outcome::CanceledNotRefunded) {
            residue -= lesson->cost;
        }
    }
    return residue;
}

QJsonObject Student::toJson() const
{
    QJsonObject json;
    json["name"] = name;
    json["grossPayments"] = (int)grossPayments;
    QJsonArray jsonLessons;
    for (std::shared_ptr<Lesson> lesson : lessons) {
        jsonLessons.append(lesson->toJson());
    }
    json["lessons"] = jsonLessons;


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


