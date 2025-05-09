#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QTime>
#include <QColor>


namespace Constants {
    // Working hours
    const QTime startTime {11, 00};
    const QTime finishTime {20, 00};

    // Filename
    const QString saveFilename = "../../sampo.save";

    // Enum to text
    const QMap<int, QString> outcomeEtoT = {{0, "Ещё не прошёл"},    // NotFinishedYet
                                            {1, "Успешно прошёл"},    // Successful
                                            {2, "Отменен, оплата удержана"},    // CanceledNotRefunded
                                            {3, "Отменен, оплата возвращена"},};  // CanceledRefunded

    // Colour maps
    const QMap<int, QColor> outcomeCM = {{0, QColor("#CED7DB")},    // NotFinishedYet
                                         {1, QColor("#B8FF98")},    // Successful
                                         {2, QColor("#FFA4B5")},    // CanceledNotRefunded
                                         {3, QColor("#FFE0B2")},};  // CanceledRefunded

}

#endif // CONSTANTS_H
