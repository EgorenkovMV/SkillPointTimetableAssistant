#include "partilist.h"

PartiListItem::PartiListItem(QListWidget *parent)
    : QListWidgetItem(parent)
{

}



PartiList::PartiList(QWidget *parent)
    : QListWidget(parent)
{ }

void PartiList::dropEvent(QDropEvent *event)
{
    QStringList priorParti;
    for (int i = 0; i < count(); ++i) {
        priorParti.append(item(i)->text());
    }

    QListWidget::dropEvent(event);

    QStringList newParti;
    for (int i = 0; i < count(); ++i) {
        if (!priorParti.contains(item(i)->text())) {
            newParti.append(item(i)->text());
        }
    }
    emit addedParti(newParti);
}
