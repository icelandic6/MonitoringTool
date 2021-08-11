#pragma once

#include <QObject>
#include <QAbstractSocket>

#include "ServerMonitor.h"

namespace net
{
    class TCPServerMonitorPrivate;
    class TCPServerMonitor : public ServerMonitor
    {
        Q_OBJECT

    public:
        TCPServerMonitor(const QString &address, int port, QObject *parent);
        ~TCPServerMonitor() override;

        void checkServer() override;

    private:
        QScopedPointer<TCPServerMonitorPrivate> d_ptr;
        Q_DISABLE_COPY(TCPServerMonitor)
        Q_DECLARE_PRIVATE(TCPServerMonitor)
    };
}
