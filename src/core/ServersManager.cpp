#include "ServersManager.h"

#include "net/TCPServerMonitor.h"
#include "net/UDPServerMonitor.h"
#include "net/ICMPServerMonitor.h"

#include "Server.h"

#include <QTimer>


class core::ServersManagerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ServersManager)
    Q_DISABLE_COPY(ServersManagerPrivate)
    ServersManager *q_ptr = nullptr;

    QList<Server*> _servers;

    QTimer _timer;

    int _frequency = 8000;
    int _sensitivity = 3;

public:
    explicit ServersManagerPrivate(ServersManager *q)
        : q_ptr(q)
    {
    }

    ~ServersManagerPrivate() {}

    void runServersCheck()
    {
        qDebug() << "\n======= ServerMonitorsManager:runServersCheck: started ========\n";

        for (auto const& server : _servers)
        {
            qDebug() << "\n======= ServerMonitorsManager: check " 
                << server->name()
                << "\n";
            server->runCheck();

        }
    }

    void addServer(Server *server)
    {
        _servers.append(server);

        connect(server, &Server::statusChanged, this, [this]()
        {
            Q_Q(ServersManager);

            auto server = static_cast<Server*>(sender());

            emit q->serverStatusUpdated(server->id(), server->status());
        });
    }
};


core::ServersManager::ServersManager(int frequencySeconds, int sensitivity, QObject *parent)
    : QObject(parent)
    , d_ptr(new ServersManagerPrivate(this))
{
    Q_D(ServersManager);

    d->_frequency = frequencySeconds;
    d->_sensitivity = sensitivity;

    d->_timer.setInterval(d->_frequency);
    d->_timer.setSingleShot(false);

    connect(&d->_timer, &QTimer::timeout, d, &ServersManagerPrivate::runServersCheck);

    d->_timer.start();
}

core::ServersManager::~ServersManager() = default;

ushort core::ServersManager::addTCPServer(const QString &name, const QString &address, int port)
{
    Q_D(ServersManager);

    auto server = new Server(name, new TCPServerMonitor(address, port, this), d->_sensitivity);
    d->addServer(server);

    return server->id();
}

ushort core::ServersManager::addUDPServer(const QString &name, const QString &address, int port)
{
    Q_D(ServersManager);

    auto server = new Server(name, new UDPServerMonitor(address, port, this), d->_sensitivity);
    d->addServer(server);

    return server->id();
}

ushort core::ServersManager::addICMPServer(const QString &name, const QString &address)
{
    Q_D(ServersManager);

    auto server = new Server(name, new ICMPServerMonitor(address, this), d->_sensitivity);
    d->addServer(server);

    return server->id();
}

// void core::ServersManager::addServerMonitor(ServerMonitor *monitor)
// {
//     Q_D(ServersManager);
// 
//     d->_servers.append(monitor);
//     connect(monitor, &ServerMonitor::finished, d, &ServersManagerPrivate::onMonitorFinished);
// }
// 
// void core::ServersManager::addTCPServerMonitor(const QString &name, const QString &hostAddress, int port)
// {
//     Q_D(ServersManager);
// 
//     addServerMonitor(new TCPServerMonitor(name, hostAddress, port, this));
// }
// 
// void core::ServersManager::addUDPServerMonitor(const QString &name, const QString &hostAddress, int port)
// {
//     Q_D(ServersManager);
// 
//     addServerMonitor(new UDPServerMonitor(name, hostAddress, port, this));
// }
// 
// void core::ServersManager::addICMPServerMonitor(const QString &name, const QString &hostAddress)
// {
//     Q_D(ServersManager);
// 
//     addServerMonitor(new ICMPServerMonitor(name, hostAddress, this));
// }

void core::ServersManager::setSensitivity(int sensitivity)
{
    Q_D(ServersManager);

}

int core::ServersManager::sensitivity() const
{
    Q_D(const ServersManager);

    return d->_sensitivity;
}
