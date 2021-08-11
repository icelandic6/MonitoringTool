#pragma once

#include <QObject>

namespace net
{
    class ServerMonitorPrivate;
    class ServerMonitor : public QObject
    {
        Q_OBJECT

    public:
        ServerMonitor(const QString &address, QObject *parent);
        ~ServerMonitor();

        QString address() const;

        virtual void checkServer() = 0;

    signals:
        void finished(bool success, int latency = -1);

    private:
        QScopedPointer<ServerMonitorPrivate> d_ptr;
        Q_DISABLE_COPY(ServerMonitor)
        Q_DECLARE_PRIVATE(ServerMonitor)
    };
}
