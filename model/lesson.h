#ifndef LESSON_H
#define LESSON_H

#include <QDateTime>
#include <QJsonObject>


typedef int Ruble;

class Student;

class Lesson
{
public:
    enum class Outcome
    {
        NotFinishedYet = 0,
        Successful,
        CanceledNotRefunded,
        CanceledRefunded
    };

    QDateTime date;
    Ruble cost;
    Student *student;
    Outcome outcome;

    Lesson(const QDateTime &date, const Ruble &cost = 0, Student *student = nullptr);
    Lesson(const QJsonObject &json, Student *student = nullptr);
    QJsonObject toJson() const;

};

#endif // LESSON_H
