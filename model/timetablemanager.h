#ifndef TIMETABLEMANAGER_H
#define TIMETABLEMANAGER_H

#include <QMap>
#include <QString>
#include "student.h"
#include "lesson.h"

class TimetableManager
{
public:
    enum class SaveFormat {
        Json,
        Binary
    };
    QMap<QString, std::shared_ptr<Student>> students;
    std::vector<std::shared_ptr<Lesson>> lessons;

    TimetableManager();
    bool save(SaveFormat saveFormat = SaveFormat::Json);
    bool load(SaveFormat saveFormat = SaveFormat::Json);
    bool addStudent(const std::shared_ptr<Student> &newStudent);
    void cleanupData();
    Ruble paymentBalance(const std::shared_ptr<Student> &student);

};

#endif // TIMETABLEMANAGER_H
