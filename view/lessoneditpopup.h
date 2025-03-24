#ifndef LESSONEDITPOPUP_H
#define LESSONEDITPOPUP_H

#include <QWidget>
#include "../model/timetablemanager.h"

namespace Ui {
class LessonEditPopup;
}

class LessonEditPopup : public QWidget
{
    Q_OBJECT

public:
    explicit LessonEditPopup(QWidget *parent = nullptr,
                             const std::shared_ptr<TimetableManager> &ttmng = nullptr,
                             const std::shared_ptr<Lesson> &lesson = nullptr,
                             const QDate &date = QDate::currentDate());
    ~LessonEditPopup();

signals:
    void lessonEdited(const std::shared_ptr<Lesson> &lesson);
    void lessonCreated(const std::shared_ptr<Lesson> &lesson);
    void canceled();

private slots:
    void saveLesson();

private:
    Ui::LessonEditPopup *ui;
    std::shared_ptr<TimetableManager> ttmng;
    std::shared_ptr<Lesson> lesson;
    enum class PopupTask {
        Edit,
        Create
    } task;

    void closeEvent(QCloseEvent *event) override;
};

#endif // LESSONEDITPOPUP_H
