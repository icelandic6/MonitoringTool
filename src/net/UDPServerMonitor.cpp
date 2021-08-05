#include "UDPServerMonitor.h"

#include <QtNetwork/QUdpSocket>

class net::UDPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(UDPServerMonitor)
    Q_DISABLE_COPY(UDPServerMonitorPrivate)
    UDPServerMonitor *q_ptr = nullptr;

    QAbstractSocket* _socket = nullptr;
    int _port = 0;

public:
    explicit UDPServerMonitorPrivate(UDPServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~UDPServerMonitorPrivate() = default;
};

net::UDPServerMonitor::UDPServerMonitor(const QString &address, int port, QObject *parent)
    : ServerMonitor(address, parent)
    , d_ptr(new UDPServerMonitorPrivate(this))
{
    Q_D(UDPServerMonitor);

    d->_socket = new QUdpSocket(this);
    d->_port = port;

    qRegisterMetaType<QUdpSocket::SocketError>("SocketError");

    connect(d->_socket, &QUdpSocket::connected, this, [this]()
    {
        Q_D(UDPServerMonitor);

        d->_socket->close();
        emit finished(true);
    });

    connect(d->_socket, SIGNAL(error(SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
}

net::UDPServerMonitor::~UDPServerMonitor() = default;

void net::UDPServerMonitor::checkServer()
{
    Q_D(UDPServerMonitor);

    d->_socket->abort();
    d->_socket->connectToHost(address(), d->_port);
}

void net::UDPServerMonitor::onError(QAbstractSocket::SocketError socketError)
{
    Q_D(UDPServerMonitor);

    d->_socket->close();

    emit finished(false);
}
