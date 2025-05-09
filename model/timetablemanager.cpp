#include "timetablemanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QCborValue>
#include <QCborMap>
#include <QFile>
#include "../constants.h"


TimetableManager::TimetableManager()
{
    // Student *s1 = new Student{"Вяйнямёйнен"};
    // s1->addPayment(1111);
    // s1->educationalPlan = "Super Minds 2";
    // s1->educationalPlanProgress = "Unit 5";
    // students.insert("Вяйнямёйнен", std::shared_ptr<Student> {s1});

    // Student *s2 = new Student{"Йоукахайнен"};
    // s2->addPayment(2222);
    // students.insert("Йоукахайнен", std::shared_ptr<Student> {s2});

    // Student *s3 = new Student{"Ильмаринен"};
    // s3->addPayment(3333);
    // s3->educationalPlanProgress = "Отстает";
    // students.insert("Ильмаринен", std::shared_ptr<Student> {s3});

    // Student *s4 = new Student{"старуха Лоухи"};
    // s4->addPayment(666);
    // s4->educationalPlan = "Верещагина 2 класс";
    // students.insert("старуха Лоухи", std::shared_ptr<Student> {s4});


    // Lesson *l1 = new Lesson{QDateTime {QDate {2025, 2, 13}, QTime {16, 45}}, 25, Lesson::Type::EnglishGroup, 1200};

    // // One have to add participants to the lesson manually
    // ParticipantInfo *l1pi1 = new ParticipantInfo{students.value("Вяйнямёйнен"), 1300, ParticipantInfo::Outcome::NotFinishedYet};
    // l1->participants.push_back(std::shared_ptr<ParticipantInfo> {l1pi1});

    // ParticipantInfo *l1pi2 = new ParticipantInfo{students.value("Йоукахайнен"), 0, ParticipantInfo::Outcome::NotFinishedYet};
    // l1->participants.push_back(std::shared_ptr<ParticipantInfo> {l1pi2});

    // lessons.push_back(std::shared_ptr<Lesson> {l1});


    // Lesson *l2 = new Lesson{QDateTime {QDate {2025, 2, 13}, QTime {19, 00}}, 90, Lesson::Type::EnglishIndividual, 1800};

    // ParticipantInfo *l2pi1 = new ParticipantInfo{students.value("старуха Лоухи"), 1300, ParticipantInfo::Outcome::NotFinishedYet};
    // l2->participants.push_back(std::shared_ptr<ParticipantInfo> {l2pi1});

    // lessons.push_back(std::shared_ptr<Lesson> {l2});


    // save();
}

bool TimetableManager::save(SaveFormat saveFormat)
{
    QJsonObject json;

    QJsonArray jsonStudents;
    QMap<QString, std::shared_ptr<Student>>::const_iterator it = students.constBegin();
    while (it != students.constEnd()) {
        jsonStudents.append(it.value()->toJson());
        ++it;
    }
    json["students"] = jsonStudents;


    QJsonArray jsonLessons;
    for (const std::shared_ptr<Lesson> &lesson : lessons) {
        jsonLessons.append(lesson->toJson());
    }
    json["lessons"] = jsonLessons;


    QFile saveFile(saveFormat == SaveFormat::Json ? Constants::saveFilename + ".json" : Constants::saveFilename + ".dat");

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("TimetableManager::save: couldn't open save file.");
        return false;
    }

    saveFile.write(saveFormat == SaveFormat::Json ? QJsonDocument(json).toJson()
                                                  : QCborValue::fromJsonValue(json).toCbor());
    return true;
}

bool TimetableManager::load(SaveFormat saveFormat)
{
    QFile loadFile(saveFormat == SaveFormat::Json ? Constants::saveFilename + ".json" : Constants::saveFilename + ".dat");

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("TimetableManager::load: couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(saveFormat == SaveFormat::Json
                              ? QJsonDocument::fromJson(saveData)
                              : QJsonDocument(QCborValue::fromCbor(saveData).toMap().toJsonObject()));

    if (!loadDoc.isObject()) {
        qWarning("TimetableManager::load: save is not an object.");
        return false;
    }

    // Loading students
    if (loadDoc.object()["students"] == QJsonValue::Null or not loadDoc.object()["students"].isArray()) {
        qWarning("TimetableManager::load: students field is malformed.");
        return false;
    }

    for (const QJsonValue &student : loadDoc.object()["students"].toArray()) {
        if (!student.isObject()) {
            qWarning("TimetableManager::load: student is not an object.");
            return false;
        }
        Student studentObj = Student::fromJson(student.toObject());
        addStudent(std::make_shared<Student>(studentObj));
    }

    // Loading lessons
    // Must load students before lessons to fill participants in latter
    if (loadDoc.object()["lessons"] == QJsonValue::Null or not loadDoc.object()["lessons"].isArray()) {
        qWarning("TimetableManager::load: lessons field is malformed.");
        return false;
    }

    for (const QJsonValue &lesson : loadDoc.object()["lessons"].toArray()) {
        if (!lesson.isObject()) {
            qWarning("TimetableManager::load: lesson is not an object.");
            return false;
        }
        Lesson lessonObj = Lesson::fromJson(lesson.toObject(), students);
        lessons.push_back(std::make_shared<Lesson>(lessonObj));
    }

    return true;
}

bool TimetableManager::addStudent(const std::shared_ptr<Student> &newStudent)
{
    if (students.count(newStudent->name)) {
        qWarning("TimetableManager::addStudent: unable to add student - name is not unique");
        return false;
    }

    students.insert(newStudent->name, newStudent);
    return true;
}

void TimetableManager::cleanupData()
{
    // Remove lessons outside the working hours in future
    size_t lessonsBefore = lessons.size();
    lessons.erase(remove_if(lessons.begin(), lessons.end(), [&] (const std::shared_ptr<Lesson> &lesson) {
                      if ((lesson->date.time() < Constants::startTime or
                           lesson->date.time().addSecs(lesson->duration * 60) > Constants::finishTime) and
                          lesson->date.date() >= QDate::currentDate()) {
                          return true;
                      }
                      return false;
                  }), lessons.end());

    if (lessonsBefore < lessons.size()) {
        qWarning("TimetableManager::cleanupData: removed some lessons outside the working hours");
    }

    // check if cached payments match payment history
    for (const std::shared_ptr<Student>& student : students) {
        student->checkCachedPayments();
    }
}

Ruble TimetableManager::paymentBalance(const std::shared_ptr<Student> &student)
{
    Ruble balance = student->totalPayments();
    for (const std::shared_ptr<Lesson> &lesson : lessons) {
        std::shared_ptr<ParticipantInfo> parti = lesson->findParticipant(student);
        if (!parti) {
            continue;
        }
        if (parti->outcome == ParticipantInfo::Outcome::Successful or
            parti->outcome == ParticipantInfo::Outcome::CanceledNotRefunded) {
            balance -= parti->individualCost;
        }
    }

    return balance;
}

void TimetableManager::deleteLessonForGood(const std::shared_ptr<Lesson> &lesson)
{
    for (const std::shared_ptr<ParticipantInfo> &parti : lesson->participants) {
        if (parti->outcome == ParticipantInfo::Outcome::CanceledRefunded or
            parti->outcome == ParticipantInfo::Outcome::NotFinishedYet) {
            continue;
        }
        parti->student->addPayment(-1 * parti->individualCost, lesson->date);
    }
    lessons.erase(std::find(lessons.begin(), lessons.end(), lesson));
}

void TimetableManager::updateLessonsOutcome()
{
    for (const std::shared_ptr<Lesson> &lesson : lessons) {
        if (lesson->date > QDateTime::currentDateTime()) {
            continue;
        }
        for (const std::shared_ptr<ParticipantInfo> &parti : lesson->participants) {
            if (parti->outcome == ParticipantInfo::Outcome::NotFinishedYet) {
                parti->outcome = ParticipantInfo::Outcome::Successful;
            }
        }
    }
}





