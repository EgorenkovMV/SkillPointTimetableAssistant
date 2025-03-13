#include "lesson.h"
#include <QJsonArray>


ParticipantInfo::ParticipantInfo(const std::shared_ptr<Student> &student, Ruble individualCost, Outcome outcome)
    : student(student)
    , individualCost(individualCost)
    , outcome(outcome)
{
    if (!student) {
        qWarning("ParticipantInfo::ParticipantInfo: student is nullptr");
    }
}

QJsonObject ParticipantInfo::toJson() const
{
    QJsonObject json;
    if (!student) {
        return json;
    }
    json["studentName"] = student->name;
    json["individualCost"] = individualCost;
    json["outcome"] = static_cast<int>(outcome);
    return json;
}

ParticipantInfo ParticipantInfo::fromJson(const QJsonObject &json, const std::shared_ptr<Student> &student)
{
    if (!student) {
        throw std::invalid_argument("ParticipantInfo::fromJson: student is nullptr");
    }

    ParticipantInfo participant {student};
    participant.individualCost = json["individualCost"].toInt();
    participant.outcome = static_cast<Outcome>(json["outcome"].toInt());
    return participant;
}


Lesson::Lesson(const QDateTime &date, Minute duration, Type type, Ruble cost)
    : date(date)
    , duration(duration)
    , type(type)
    , cost(cost)
{}

QJsonObject Lesson::toJson() const
{
    QJsonObject json;
    json["date"] = date.toString();
    json["duration"] = duration;
    json["type"] = static_cast<int>(type);
    json["cost"] = cost;

    QJsonArray jsonParticipants;
    for (const std::shared_ptr<ParticipantInfo> &participant : participants) {
        jsonParticipants.append(participant->toJson());
    }
    json["participants"] = jsonParticipants;
    json["additionalInfo"] = additionalInfo;
    return json;
}

/// Requires full map of students
Lesson Lesson::fromJson(const QJsonObject &json, const QMap<QString, std::shared_ptr<Student>> &students)
{
    Lesson lesson;
    lesson.date = QDateTime::fromString(json.value("date").toString());
    lesson.duration = json.value("duration").toInt();
    lesson.type = static_cast<Type>(json.value("type").toInt());
    lesson.cost = json.value("cost").toInt();

    for (const QJsonValue &participant : json.value("participants").toArray()) {
        QString studentName = participant.toObject()["studentName"].toString();
        ParticipantInfo info = ParticipantInfo::fromJson(participant.toObject(),
                                                         students.value(studentName, nullptr));

        lesson.participants.push_back(std::make_shared<ParticipantInfo>(info));
    }
    lesson.additionalInfo = json.value("additionalInfo").toString();
    return lesson;
}



