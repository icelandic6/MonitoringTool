#pragma once

#include <QObject>
#include <QAbstractSocket>

#include "ServerMonitor.h"

namespace net
{
    class UDPServerMonitorPrivate;
    class UDPServerMonitor : public ServerMonitor
    {
        Q_OBJECT

    public:
        UDPServerMonitor(const QString &address, int port, QObject *parent);
        ~UDPServerMonitor() override;

        void checkServer() override;

    private:
        QScopedPointer<UDPServerMonitorPrivate> d_ptr;
        Q_DISABLE_COPY(UDPServerMonitor)
        Q_DECLARE_PRIVATE(UDPServerMonitor)
    };
}
