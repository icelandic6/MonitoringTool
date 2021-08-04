#pragma once

#include <QObject>

#include "ServerStatus.h"

class ServerMonitor;

namespace core
{
    class ServerPrivate;
    class Server : public QObject
    {
        Q_OBJECT

    public:
        Server(const QString &name, ServerMonitor *monitor, QObject *parent = nullptr);
        ~Server();

        void runCheck();

        ushort id() const;
        QString name() const;
        ServerStatus status() const;
        ServerStatus prevStatus() const;

    signals:
        void statusChanged();
        void latencyChanged(int value);

    private:
        QScopedPointer<ServerPrivate> d_ptr;
        Q_DISABLE_COPY(Server)
        Q_DECLARE_PRIVATE(Server)
    };
}