#ifndef LESSONEDITPOPUP_H
#define LESSONEDITPOPUP_H

#include <QWidget>
#include <QListWidget>
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
    void lessonDeleted(const std::shared_ptr<Lesson> &lesson);
    void canceled();

private slots:
    void saveLesson();
    void addParti(const QStringList &partiList);
    void addPartiButton();
    void removeParti();

private:
    Ui::LessonEditPopup *ui;
    std::shared_ptr<TimetableManager> ttmng;
    std::shared_ptr<Lesson> lesson;
    QDate date;
    enum class PopupTask {
        Edit,
        Create
    } task;

    std::vector<std::shared_ptr<ParticipantInfo>> participants;

    void setPartiSideData(const std::shared_ptr<ParticipantInfo> &parti);

    void closeEvent(QCloseEvent *event) override;

};

#endif // LESSONEDITPOPUP_H
