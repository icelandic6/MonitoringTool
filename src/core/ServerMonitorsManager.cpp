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

    int _frequency = 8000;
    int _sensitivity = 5;

public:
    explicit ServerMonitorsManagerPrivate(ServerMonitorsManager *q)
        : q_ptr(q)
    {
    }

    ~ServerMonitorsManagerPrivate() {}

    void runServersCheck()
    {
        qDebug() << "\n======= ServerMonitorsManager:runServersCheck: started ========\n";

        for (auto const& monitor : _monitors)
        {
            qDebug() << "\n======= ServerMonitorsManager: check " 
                << monitor->name()
                << "\n";
            monitor->checkServer();
        }
    }

    void onMonitorFinished(bool success)
    {
        auto monitor = static_cast<ServerMonitor*>(sender());
        qDebug() << "\n======= ServerMonitorsManager:onMonitorFinished: " << monitor->name() << " was "
            << (success ? "successful" : "failed");
    }
};


core::ServerMonitorsManager::ServerMonitorsManager(int frequencySeconds, QObject *parent)
    : QObject(parent)
    , d_ptr(new ServerMonitorsManagerPrivate(this))
{
    Q_D(ServerMonitorsManager);

    d->_frequency = frequencySeconds;
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
    connect(monitor, &ServerMonitor::finished, d, &ServerMonitorsManagerPrivate::onMonitorFinished);
}

void core::ServerMonitorsManager::addTCPServerMonitor(const QString &name, const QString &hostAddress, int port)
{
    Q_D(ServerMonitorsManager);

    addServerMonitor(new TCPServerMonitor(name, hostAddress, port, this));
}

void core::ServerMonitorsManager::addUDPServerMonitor(const QString &name, const QString &hostAddress, int port)
{
    Q_D(ServerMonitorsManager);

    addServerMonitor(new UDPServerMonitor(name, hostAddress, port, this));
}

void core::ServerMonitorsManager::addICMPServerMonitor(const QString &name, const QString &hostAddress)
{
    Q_D(ServerMonitorsManager);

    addServerMonitor(new ICMPServerMonitor(name, hostAddress, this));
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
