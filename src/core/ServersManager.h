#pragma once

#include <QObject>

#include "ServerStatus.h"

namespace core
{
    class ServersManagerPrivate;
    class ServersManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ServersManager(int frequencySeconds, int sensitivity, QObject *parent = nullptr);
        ~ServersManager();

        void addTCPServer(const QString &name, const QString &address, int port);
        void addUDPServer(const QString &name, const QString &address, int port);
        void addICMPServer(const QString &name, const QString &address);

//         void addServerMonitor(ServerMonitor *monitor);
//         void addTCPServerMonitor(const QString &name, const QString &hostAddress, int port);
//         void addUDPServerMonitor(const QString &name, const QString &hostAddress, int port);
//         void addICMPServerMonitor(const QString &name, const QString &hostAddress);

        void setSensitivity(int sensitivity);
        int sensitivity() const;

    signals:
        void serverStatusUpdated(uint serverId, ServerStatus status);

    private:
        QScopedPointer<ServersManagerPrivate> d_ptr;
        Q_DISABLE_COPY(ServersManager)
        Q_DECLARE_PRIVATE(ServersManager)
    };
}