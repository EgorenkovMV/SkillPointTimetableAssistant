#include "lesson.h"

Lesson::Lesson(const QDateTime &date, const Ruble &cost, Student *student)
    : date(date)
    , cost(cost)
    , student(student)
    , outcome(Outcome::NotFinishedYet)
{}

Lesson::Lesson(const QJsonObject &json, Student *student)
{
    date = QDateTime::fromString(json.value("date").toString());
    cost = json.value("cost").toInt();
    this->student = student;
    outcome = static_cast<Outcome>(json.value("outcome").toInt());
}


QJsonObject Lesson::toJson() const
{
    QJsonObject json;
    json["date"] = date.toString();
    json["cost"] = cost;
    json["outcome"] = static_cast<int>(outcome);
    return json;
}



