#include "ServerMonitor.h"

class net::ServerMonitorPrivate : public QObject
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

net::ServerMonitor::ServerMonitor(const QString &address, QObject *parent)
    : QObject(parent)
    , d_ptr(new ServerMonitorPrivate(this))
{
    Q_D(ServerMonitor);

    d->_address = address;
}

net::ServerMonitor::~ServerMonitor() = default;

QString net::ServerMonitor::address() const
{
    Q_D(const ServerMonitor);

    return d->_address;
}
