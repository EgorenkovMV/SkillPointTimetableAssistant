#include "timetablemanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QCborValue>
#include <QCborMap>
#include <QFile>

namespace {
QString saveFilename = "../../sampo.save";
}

TimetableManager::TimetableManager()
{
    // Student s1 {"Вяйнямёйнен"};
    // s1.addPayment(1111);
    // students.push_back(s1);

    // Student s2 {"Йоукахайнен"};
    // s2.addPayment(2222);
    // students.push_back(s2);

    // Student s3 {"Ильмаринен"};
    // s3.addPayment(3333);
    // students.push_back(s3);

    // Student s4 {"старуха Лоухи"};
    // s4.addPayment(666);
    // students.push_back(s4);


    // students[0].lessons.push_back(std::shared_ptr<Lesson> {new Lesson(QDateTime::currentDateTime(), 1500, &students[0])});
    // students[1].lessons.push_back(std::shared_ptr<Lesson> {new Lesson(QDateTime::currentDateTime(), 1000, &students[1])});
    // students[2].lessons.push_back(std::shared_ptr<Lesson> {new Lesson(QDateTime::currentDateTime(), 1200, &students[2])});

    // save();
}

bool TimetableManager::save(SaveFormat saveFormat)
{
    QJsonArray jsonStudents;
    for (const Student &student : students) {
        jsonStudents.append(student.toJson());
    }

    QFile saveFile(saveFormat == SaveFormat::Json ? saveFilename + ".json" : saveFilename + ".dat");

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("TimetableManager::save: couldn't open save file.");
        return false;
    }

    saveFile.write(saveFormat == SaveFormat::Json ? QJsonDocument(jsonStudents).toJson()
                                                  : QCborValue::fromJsonValue(jsonStudents).toCbor());
    return true;
}

bool TimetableManager::load(SaveFormat saveFormat)
{
    QFile loadFile(saveFormat == SaveFormat::Json ? saveFilename + ".json" : saveFilename + ".dat");

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("TimetableManager::load: couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(saveFormat == SaveFormat::Json
                              ? QJsonDocument::fromJson(saveData)
                              : QJsonDocument(QCborValue::fromCbor(saveData).toMap().toJsonObject()));

    if (!loadDoc.isArray()) {
        qWarning("TimetableManager::load: save is not an array.");
        return false;
    }

    for (const QJsonValue &student : loadDoc.array()) {
        if (!student.isObject()) {
            qWarning("TimetableManager::load: save array is malformed.");
            return false;
        }
        students.push_back(student.toObject());
    }

    return true;
}



