#include "ServersManager.h"
#include "AppSettings.h"
#include "Server.h"

#include "net/TCPServerMonitor.h"
#include "net/UDPServerMonitor.h"
#include "net/ICMPServerMonitor.h"

#include <QTimer>

class core::ServersManagerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ServersManager)
    Q_DISABLE_COPY(ServersManagerPrivate)
    ServersManager *q_ptr = nullptr;

    QList<Server*> _servers;
    Server *_worstServer = nullptr;

    QTimer _timer;

public:
    explicit ServersManagerPrivate(ServersManager *q)
        : q_ptr(q)
    {
    }

    ~ServersManagerPrivate() = default;

    void runServersCheck()
    {
        for (auto const& server : _servers)
            server->runCheck();
    }

    void addServer(Server *server)
    {
        _servers.append(server);
        connect(server, &Server::statusChanged, this, &ServersManagerPrivate::onStatusChanged);
        connect(server, &Server::latencyChanged, this, &ServersManagerPrivate::onLatencyChanged);
    }

    void onStatusChanged()
    {
        Q_Q(ServersManager);

        auto server = static_cast<Server*>(sender());
        checkIfWorstServer(server);

        emit q->serverStatusUpdated(server->id(), server->status());

        if (_worstServer == nullptr)
            setWorstServer(server);
    }

    void onLatencyChanged(int latency)
    {
        Q_Q(ServersManager);

        auto server = static_cast<Server*>(sender());

        if (latency != -1)
            emit q->serverLatencyUpdated(server->id(), latency);
    }

    void setWorstServer(Server *server)
    {
        Q_Q(ServersManager);

        _worstServer = server;
        emit q->worstServerUpdated(_worstServer->id(), _worstServer->status());
    }

    void checkIfWorstServer(Server *server)
    {
        if (_worstServer == nullptr)
        {
            setWorstServer(server);
            return;
        }

        if (server->id() == _worstServer->id())
        {
            for (auto &s : _servers)
                if (s->status() < _worstServer->status())
                {
                    setWorstServer(s);
                    return;
                }
            setWorstServer(server);
        }
        else if (server->status() < _worstServer->status())
            setWorstServer(server);
    }
};


core::ServersManager::ServersManager(QObject *parent)
    : QObject(parent)
    , d_ptr(new ServersManagerPrivate(this))
{
    Q_D(ServersManager);

    connect(&d->_timer, &QTimer::timeout, d, &ServersManagerPrivate::runServersCheck);
}

core::ServersManager::~ServersManager() = default;

ushort core::ServersManager::addTCPServer(const QString &name, const QString &address, int port)
{
    Q_D(ServersManager);

    auto server = new Server(name, new net::TCPServerMonitor(address, port, this));
    d->addServer(server);

    return server->id();
}

ushort core::ServersManager::addUDPServer(const QString &name, const QString &address, int port)
{
    Q_D(ServersManager);

    auto server = new Server(name, new net::UDPServerMonitor(address, port, this));
    d->addServer(server);

    return server->id();
}

ushort core::ServersManager::addICMPServer(const QString &name, const QString &address)
{
    Q_D(ServersManager);

    auto server = new Server(name, new net::ICMPServerMonitor(address, this));
    d->addServer(server);

    return server->id();
}

void core::ServersManager::startMonitoring()
{
    Q_D(ServersManager);

    d->_timer.setInterval(AppSettings::instance()->frequencySeconds() * 1000);
    d->_timer.setSingleShot(false);
    d->_timer.start();
}
