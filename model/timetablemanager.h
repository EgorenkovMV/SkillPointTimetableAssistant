#ifndef TIMETABLEMANAGER_H
#define TIMETABLEMANAGER_H

#include "student.h"
#include "lesson.h"

class TimetableManager
{
public:
    enum class SaveFormat {
        Json,
        Binary
    };
    std::vector<Student> students;

    TimetableManager();
    bool save(SaveFormat saveFormat = SaveFormat::Json);
    bool load(SaveFormat saveFormat = SaveFormat::Json);


};

#endif // TIMETABLEMANAGER_H
