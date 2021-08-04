#include "ServerMonitor.h"

class ServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ServerMonitor)
    Q_DISABLE_COPY(ServerMonitorPrivate)
    ServerMonitor *q_ptr = nullptr;

    QString _address;

public:
    explicit ServerMonitorPrivate(ServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~ServerMonitorPrivate() = default;
};

ServerMonitor::ServerMonitor(const QString &address, QObject *parent)
    : QObject(parent)
    , d_ptr(new ServerMonitorPrivate(this))
{
    Q_D(ServerMonitor);

    d->_address = address;
}

ServerMonitor::~ServerMonitor() = default;

QString ServerMonitor::address() const
{
    Q_D(const ServerMonitor);

    return d->_address;
}
