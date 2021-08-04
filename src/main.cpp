#include <QtWidgets/QApplication>

#include "core/MonitoringTool.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    core::MonitoringTool mtool;
    return a.exec();
}
