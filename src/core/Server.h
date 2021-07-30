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
        explicit Server(const QString &name, ServerMonitor *monitor, short sensitivity, QObject *parent = nullptr);
        ~Server();

        void runCheck();

        uint id() const;
        QString name() const;
        ServerStatus status() const;

    signals:
        void statusChanged();

    private:
        QScopedPointer<ServerPrivate> d_ptr;
        Q_DISABLE_COPY(Server)
        Q_DECLARE_PRIVATE(Server)
    };
}