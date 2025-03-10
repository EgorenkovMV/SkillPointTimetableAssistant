#include "mainwidget.h"

#include <QApplication>
#include "model/timetablemanager.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w;
    w.show();

    TimetableManager ttmng {};

    return a.exec();
}
