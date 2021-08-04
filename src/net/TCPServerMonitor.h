#pragma once

#include <QObject>
#include <QAbstractSocket>

#include "ServerMonitor.h"

class TCPServerMonitorPrivate;
class TCPServerMonitor : public ServerMonitor
{
    Q_OBJECT

public:
    TCPServerMonitor(const QString &address, int port, QObject *parent);
    ~TCPServerMonitor() override;

    void checkServer() override;

public slots:
    void onError(QAbstractSocket::SocketError socketError);

private:
    QScopedPointer<TCPServerMonitorPrivate> d_ptr;
    Q_DISABLE_COPY(TCPServerMonitor)
    Q_DECLARE_PRIVATE(TCPServerMonitor)
};
