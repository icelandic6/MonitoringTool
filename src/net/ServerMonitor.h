#pragma once

#include <QObject>

class ServerMonitorPrivate;
class ServerMonitor : public QObject
{
    Q_OBJECT

public:
    ServerMonitor(const QString &address, QObject *parent);
    virtual ~ServerMonitor();

    QString address() const;

    virtual void checkServer() = 0;

signals:
    void finished(bool success, int latency = 0);

private:
    QScopedPointer<ServerMonitorPrivate> d_ptr;
    Q_DISABLE_COPY(ServerMonitor)
    Q_DECLARE_PRIVATE(ServerMonitor)
};
