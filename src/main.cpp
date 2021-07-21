#include "MonitoringToolWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	MonitoringToolWidget w;
    w.show();
    return a.exec();
}
