#pragma once

#include <QObject>

namespace core
{
    class MonitoringToolPrivate;
    class MonitoringTool : public QObject
    {
    public:
        explicit MonitoringTool(QObject *parent = nullptr);
        ~MonitoringTool();

    private:
        QScopedPointer<MonitoringToolPrivate> d_ptr;
        Q_DISABLE_COPY(MonitoringTool)
        Q_DECLARE_PRIVATE(MonitoringTool)
    };
}