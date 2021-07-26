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

    qDebug() << QString("====== ServerMonitor: host address = [%1]").arg(hostAddress);

    d->_friendlyServerName = name;
    d->_hostAddress = hostAddress;
    d->_hostPort = port;
}

ServerMonitor::~ServerMonitor() = default;

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

// void ServerMonitor::checkServer()
// {
// 	Q_D(ServerMonitor);
// 
// 	if (d->_protocol == ServerProtocol::TCP)
// 	{
// 		qDebug() << "==== Running TCP server check";
// 		d->_socket = new QTcpSocket(this);
// 
// 		int metatype_id_2 = qRegisterMetaType<QTcpSocket::SocketError>("SocketError");
// 
// 		connect(d->_socket, &QTcpSocket::stateChanged, this, [this](QAbstractSocket::SocketState socketState)
// 		{
// 			qDebug() << QString("==== TCP CHECK: %1").arg(socketState);
// 		});
// 		connect(d->_socket, &QTcpSocket::connected, this, [this]()
// 		{
// 			qDebug() << QString("==== TCP CHECK: CONNECTED");
// 		});
// 
// 		connect(d->_socket, SIGNAL(error(SocketError)),
// 			this, SLOT(onError(QAbstractSocket::SocketError)));
// 
// 		d->_socket->connectToHost("google.comma", 80);
// 	}
// }
