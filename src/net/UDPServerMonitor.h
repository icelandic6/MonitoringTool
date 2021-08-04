#pragma once

#include <QObject>
#include <QAbstractSocket>

#include "ServerMonitor.h"

class UDPServerMonitorPrivate;
class UDPServerMonitor : public ServerMonitor
{
    Q_OBJECT

public:
    UDPServerMonitor(const QString &address, int port, QObject *parent);
    ~UDPServerMonitor() override;

    void checkServer() override;

public slots:
    void onError(QAbstractSocket::SocketError socketError);

private:
    QScopedPointer<UDPServerMonitorPrivate> d_ptr;
    Q_DISABLE_COPY(UDPServerMonitor)
    Q_DECLARE_PRIVATE(UDPServerMonitor)
};
