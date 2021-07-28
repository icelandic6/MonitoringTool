#include "ServerMonitorsManager.h"

#include "net/TCPServerMonitor.h"
#include "net/UDPServerMonitor.h"
#include "net/ICMPServerMonitor.h"

#include <QApplication>
#include <QTimer>

class core::ServerMonitorsManagerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ServerMonitorsManager)
    Q_DISABLE_COPY(ServerMonitorsManagerPrivate)
    ServerMonitorsManager *q_ptr = nullptr;

    QList<ServerMonitor*> _monitors;

    QTimer _timer;

    int _frequency = 15;
    int _sensitivity = 5;

public:
    explicit ServerMonitorsManagerPrivate(ServerMonitorsManager *q)
        : q_ptr(q)
    {
    }

    ~ServerMonitorsManagerPrivate() {}

    void runServersCheck()
    {
//         for (auto const& monitor : _monitors)
//         {
//         }
    }
};


core::ServerMonitorsManager::ServerMonitorsManager(QObject *parent)
    : QObject(parent)
    , d_ptr(new ServerMonitorsManagerPrivate(this))
{
    Q_D(ServerMonitorsManager);

    d->_timer.setInterval(d->_frequency);
    d->_timer.setSingleShot(false);

    connect(&d->_timer, &QTimer::timeout, d, &ServerMonitorsManagerPrivate::runServersCheck);

    d->_timer.start();
}

core::ServerMonitorsManager::~ServerMonitorsManager() = default;

void core::ServerMonitorsManager::addServerMonitor(ServerMonitor *monitor)
{
    Q_D(ServerMonitorsManager);

    d->_monitors.append(monitor);
}

void core::ServerMonitorsManager::addTCPServerMonitor(const QString &name, const QString &hostAddress, int port)
{
    Q_D(ServerMonitorsManager);

    d->_monitors.append(new TCPServerMonitor(name, hostAddress, 80, this));
}

void core::ServerMonitorsManager::addUDPServerMonitor(const QString &name, const QString &hostAddress, int port)
{
    Q_D(ServerMonitorsManager);

    d->_monitors.append(new UDPServerMonitor(name, hostAddress, 80, this));
}

void core::ServerMonitorsManager::addICMPServerMonitor(const QString &name, const QString &hostAddress)
{
    Q_D(ServerMonitorsManager);

    d->_monitors.append(new ICMPServerMonitor(name, hostAddress, this));
}

void core::ServerMonitorsManager::setFrequency(int frequencySeconds)
{
    Q_D(ServerMonitorsManager);

    d->_frequency = frequencySeconds;

    d->_timer.stop();
    d->_timer.setInterval(d->_frequency);
    d->_timer.start();
}

int core::ServerMonitorsManager::frequencySeconds() const
{
    Q_D(const ServerMonitorsManager);

    return d->_frequency;
}

void core::ServerMonitorsManager::setSensitivity(int sensitivity)
{
    Q_D(ServerMonitorsManager);

}

int core::ServerMonitorsManager::sensitivity() const
{
    Q_D(const ServerMonitorsManager);

    return d->_sensitivity;
}
