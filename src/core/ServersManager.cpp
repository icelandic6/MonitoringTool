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
    Server *_worstServer = nullptr;

    QTimer _timer;

    int _frequency = 5000;
    int _sensitivity = 2;

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

        connect(server, &Server::statusChanged, this, [this]()
        {
            Q_Q(ServersManager);

            auto server = static_cast<Server*>(sender());
            checkIfWorstServer(server);

            emit q->serverStatusUpdated(server->id(), server->status());
        });

        if (_worstServer == nullptr)
            setWorstServer(server);
    }

    void setWorstServer(Server *server)
    {
        Q_Q(ServersManager);

        qDebug() << "\n**** NEW WORST SERVER: [" << server->name() << "|" << (server->status() == ServerStatus::Available ? "AVAILABLE" : (server->status() == ServerStatus::Failed ? "FAILED" : "UNSTABLE")) << "]\n\n";

        _worstServer = server;
        emit q->worstServerUpdated(_worstServer->name(), _worstServer->status());
    }

    void checkIfWorstServer(Server *server)
    {
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
