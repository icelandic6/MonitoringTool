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
    Server *_leastAvailableServer = nullptr;

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

            checkIfLeastAvailable(server);

            emit q->serverStatusUpdated(server->id(), server->status());
        });

        if (_leastAvailableServer == nullptr)
            setLeastAvailableServer(server);
    }

    void setLeastAvailableServer(Server *server)
    {
        Q_Q(ServersManager);

        qDebug() << "\n**** NEW LEAST AVAILABLE SERVER: [" << server->name() << "|" << (server->status() == ServerStatus::Available ? "AVAILABLE" : (server->status() == ServerStatus::Failed ? "FAILED" : "UNSTABLE")) << "]\n\n";

        _leastAvailableServer = server;
        emit q->leastAvailableServerUpdated(
            _leastAvailableServer->name(),
            _leastAvailableServer->status());
    }

    void checkIfLeastAvailable(Server *server)
    {
        qDebug() << "\n**** "
            << "[" << _leastAvailableServer->name() << "|" << (_leastAvailableServer->status() == ServerStatus::Available ? "AVAILABLE" : (_leastAvailableServer->status() == ServerStatus::Failed ? "FAILED" : "UNSTABLE")) << "]"
            << " VS "
            << "[" << server->name() << "|" << (server->status() == ServerStatus::Available ? "AVAILABLE" : (server->status() == ServerStatus::Failed ? "FAILED" : "UNSTABLE")) << "]\n";

        if (server->id() == _leastAvailableServer->id())
        {
            for (auto &s : _servers)
                if (s->status() < _leastAvailableServer->status())
                {
                    setLeastAvailableServer(s);
                    return;
                }
            setLeastAvailableServer(server);
        }
        else if (server->status() < _leastAvailableServer->status())
            setLeastAvailableServer(server);
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

// QString core::ServersManager::serverNameById(ushort id) const
// {
//     Q_D(const ServersManager);
// 
//     auto s = std::find(d->_servers.begin(), d->_servers.end(), [id](Server *s)
//     {
//         return s->id() == id;
//     });
// 
//     return (*s)->name();
// }

QPair<ushort, core::ServerStatus> core::ServersManager::leastAvailableServer() const
{
    Q_D(const ServersManager);

    return QPair<ushort, core::ServerStatus>(d->_leastAvailableServer->id(), d->_leastAvailableServer->status());
}
