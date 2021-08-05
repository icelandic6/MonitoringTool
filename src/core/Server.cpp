#include "Server.h"
#include "AppSettings.h"
#include "net/ServerMonitor.h"

#include <QDebug>

static ushort nextId = 0;

class core::ServerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(Server)
    Q_DISABLE_COPY(ServerPrivate)
    Server *q_ptr = nullptr;

    ushort _id;
    QString _name;
    ServerStatus _status = ServerStatus::Available;
    ServerStatus _prevStatus = ServerStatus::Available;
    net::ServerMonitor* _monitor;

    short _stability = 0;
    short _sensitivity;

public:
    explicit ServerPrivate(Server *q)
        : q_ptr(q)
    {
    }

    ~ServerPrivate() = default;

    void handleCheck(bool success, int latency = 0)
    {
        qDebug() << QString("==== Server check [%1]: %2").arg(_name).arg(success);

        if (latency != 0)
        {
            Q_Q(Server);
            emit q->latencyChanged(latency);
        }

        if ((success && _status == ServerStatus::Available) ||
            (!success && _status == ServerStatus::Failed))
        {
            _stability = 0;
            return;
        }

        if (_stability < 0 && success)
            _stability = 1;
        else if (_stability > 0 && !success)
            _stability = -1;
        else
            _stability += success ? 1 : -1;

        if (_stability >= _sensitivity)
            raiseStatus();
        else if (_stability <= (-_sensitivity))
            lowerStatus();
    }

    void raiseStatus()
    {
        Q_Q(Server);

        if (_status == ServerStatus::Unstable)
        {
            _prevStatus = _status;
            _status = ServerStatus::Available;
        }
        else if (_status == ServerStatus::Failed)
        {
            _prevStatus = _status;
            _status = ServerStatus::Unstable;
        }

        _stability = 0;

        emit q->statusChanged();
    }

    void lowerStatus()
    {
        Q_Q(Server);

        if (_status == ServerStatus::Unstable)
        {
            _prevStatus = _status;
            _status = ServerStatus::Failed;
        }
        else if (_status == ServerStatus::Available)
        {
            _prevStatus = _status;
            _status = ServerStatus::Unstable;
        }

        _stability = 0;

        emit q->statusChanged();
    }
};

core::Server::Server(const QString &name, net::ServerMonitor *monitor, QObject *parent)
    : QObject(parent)
    , d_ptr(new ServerPrivate(this))
{
    Q_D(Server);

    d->_id = ::nextId++;
    d->_name = name;
    d->_monitor = monitor;
    d->_sensitivity = AppSettings::instance()->sensitivity();

    connect(d->_monitor, &net::ServerMonitor::finished, d, &ServerPrivate::handleCheck);
}

core::Server::~Server() = default;

void core::Server::runCheck()
{
    Q_D(Server);

    d->_monitor->checkServer();
}

ushort core::Server::id() const
{
    Q_D(const Server);

    return d->_id;
}

QString core::Server::name() const
{
    Q_D(const Server);

    return d->_name;
}

core::ServerStatus core::Server::status() const
{
    Q_D(const Server);

    return d->_status;
}

core::ServerStatus core::Server::prevStatus() const
{
    Q_D(const Server);

    return d->_prevStatus;
}
