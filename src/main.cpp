#include <QtWidgets/QApplication>

#include "core/MonitoringTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    core::MonitoringTool mtool;
    return a.exec();
}
