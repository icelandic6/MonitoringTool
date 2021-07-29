#include "ServerMonitor.h"

#include <QtNetwork/QTcpSocket>

class ServerMonitorPrivate : public QObject
{
	Q_DECLARE_PUBLIC(ServerMonitor)
	Q_DISABLE_COPY(ServerMonitorPrivate)
	ServerMonitor *q_ptr = nullptr;

	QString _friendlyServerName;
    QString _hostAddress;
	int _hostPort;


public:
	explicit ServerMonitorPrivate(ServerMonitor *q)
		: q_ptr(q)
	{
	}

	~ServerMonitorPrivate()
	{
	}
};

// ServerMonitor::ServerMonitor(QString name, QHostAddress hostAddress, int port, QObject *parent)
// {
// }

ServerMonitor::ServerMonitor(QString name, QString hostAddress, int port, QObject *parent)
    : QObject(parent)
    , d_ptr(new ServerMonitorPrivate(this))
{
    Q_D(ServerMonitor);

    d->_friendlyServerName = name;
    d->_hostAddress = hostAddress;
    d->_hostPort = port;
}

ServerMonitor::~ServerMonitor() = default;

QString ServerMonitor::name() const
{
    Q_D(const ServerMonitor);

    return d->_friendlyServerName;
}

QString ServerMonitor::hostAddress() const
{
    Q_D(const ServerMonitor);

    return d->_hostAddress;
}

int ServerMonitor::hostPort() const
{
    Q_D(const ServerMonitor);

    return d->_hostPort;
}
