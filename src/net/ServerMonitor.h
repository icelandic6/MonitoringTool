#pragma once

#include <QObject>
#include <QHostAddress>

enum class ServerProtocol
{
    TCP,
    UDP,
    ICMP,
};

class ServerMonitorPrivate;
class ServerMonitor : public QObject
{
    Q_OBJECT

public:
//     ServerMonitor(QString name, QHostAddress hostAddress, int port, QObject *parent);
    ServerMonitor(QString name, QString hostAddress, int port, QObject *parent);
    virtual ~ServerMonitor();

    QString name() const;
    QString hostAddress() const;
    //TODO: remove to TCP and UDP children as well as port arg in ctor
    int hostPort() const;

    virtual void checkServer() = 0;

signals:
    void finished(bool success);

private:
    QScopedPointer<ServerMonitorPrivate> d_ptr;
    Q_DISABLE_COPY(ServerMonitor)
    Q_DECLARE_PRIVATE(ServerMonitor)
};
