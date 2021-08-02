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

        ushort addTCPServer(const QString &name, const QString &address, int port);
        ushort addUDPServer(const QString &name, const QString &address, int port);
        ushort addICMPServer(const QString &name, const QString &address);

    signals:
        void serverStatusUpdated(ushort serverId, ServerStatus status);
        void worstServerUpdated(const QString &name, ServerStatus status);

    private:
        QScopedPointer<ServersManagerPrivate> d_ptr;
        Q_DISABLE_COPY(ServersManager)
        Q_DECLARE_PRIVATE(ServersManager)
    };
}