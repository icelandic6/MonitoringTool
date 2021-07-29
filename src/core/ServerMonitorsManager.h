#pragma once

#include <QObject>

#include "net/ServerMonitor.h"

namespace core
{
    class ServerMonitorsManagerPrivate;
    class ServerMonitorsManager : public QObject
    {
        Q_OBJECT

    public:
        explicit ServerMonitorsManager(int frequencySeconds, QObject *parent = nullptr);
        ~ServerMonitorsManager();

        void addServerMonitor(ServerMonitor *monitor);
        void addTCPServerMonitor(const QString &name, const QString &hostAddress, int port);
        void addUDPServerMonitor(const QString &name, const QString &hostAddress, int port);
        void addICMPServerMonitor(const QString &name, const QString &hostAddress);

        void setFrequency(int frequencySeconds);
        int frequencySeconds() const;

        void setSensitivity(int sensitivity);
        int sensitivity() const;

    private:
        QScopedPointer<ServerMonitorsManagerPrivate> d_ptr;
        Q_DISABLE_COPY(ServerMonitorsManager)
        Q_DECLARE_PRIVATE(ServerMonitorsManager)
    };
}