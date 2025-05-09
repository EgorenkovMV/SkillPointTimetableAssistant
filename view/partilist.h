#ifndef PARTILIST_H
#define PARTILIST_H

#include <QListWidget>
#include <QDropEvent>


class PartiList : public QListWidget {
    Q_OBJECT

public:
    explicit PartiList(QWidget *parent = nullptr);

signals:
    void addedParti(const QStringList &parti);

private:
    virtual void dropEvent(QDropEvent *event) override;

};
#endif // PARTILIST_H
