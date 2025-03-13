#ifndef LESSON_H
#define LESSON_H

#include <QDateTime>
#include <QString>
#include <QJsonObject>
#include <memory>
#include "student.h"


class ParticipantInfo
{
public:
    std::shared_ptr<Student> student;
    Ruble individualCost;
    enum class Outcome
    {
        NotFinishedYet = 0,
        Successful,
        CanceledNotRefunded,
        CanceledRefunded
    } outcome;

    ParticipantInfo(const std::shared_ptr<Student> &student, Ruble individualCost = 0, Outcome outcome = Outcome::NotFinishedYet);
    QJsonObject toJson() const;
    static ParticipantInfo fromJson(const QJsonObject &json, const std::shared_ptr<Student> &student);
};


class Lesson
{
public:
    enum class Type
    {
        EnglishIndividual = 0,
        EnglishGroup,
    };

    QDateTime date;
    Minute duration;
    Type type;
    Ruble cost;
    std::vector<std::shared_ptr<ParticipantInfo>> participants;
    QString additionalInfo;

    Lesson(const QDateTime &date = QDateTime::currentDateTime(), Minute duration = 0, Type type = Type::EnglishIndividual, Ruble cost = 0);
    QJsonObject toJson() const;
    static Lesson fromJson(const QJsonObject &json, const QMap<QString, std::shared_ptr<Student>> &students);
};

#endif // LESSON_H
