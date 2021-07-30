#pragma once

#include <QObject>
#include <QHostAddress>

#include "ServerMonitor.h"

class ICMPServerMonitorPrivate;
class ICMPServerMonitor : public ServerMonitor
{
    Q_OBJECT

public:
    ICMPServerMonitor(const QString &address, QObject *parent);
    virtual ~ICMPServerMonitor() override;

    void checkServer() override;

private:
    QScopedPointer<ICMPServerMonitorPrivate> d_ptr;
    Q_DISABLE_COPY(ICMPServerMonitor)
    Q_DECLARE_PRIVATE(ICMPServerMonitor)
};
