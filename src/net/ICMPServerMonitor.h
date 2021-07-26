#pragma once

#include <QObject>
#include <QHostAddress>

#include "ServerMonitor.h"

class ICMPServerMonitorPrivate;
class ICMPServerMonitor : public ServerMonitor
{
    Q_OBJECT

public:
    ICMPServerMonitor(QString name, QString hostAddress, int port, QObject *parent);
    virtual ~ICMPServerMonitor() override;

    void checkServer() override;

public slots:
    void onError(QAbstractSocket::SocketError socketError);

private:
    QScopedPointer<ICMPServerMonitorPrivate> d_ptr;
    Q_DISABLE_COPY(ICMPServerMonitor)
    Q_DECLARE_PRIVATE(ICMPServerMonitor)
};
