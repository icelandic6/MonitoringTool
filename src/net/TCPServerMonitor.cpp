#include "TCPServerMonitor.h"

#include <QtNetwork/QTcpSocket>
#include <QTimer>

class net::TCPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(TCPServerMonitor)
    Q_DISABLE_COPY(TCPServerMonitorPrivate)
    TCPServerMonitor *q_ptr = nullptr;

    QAbstractSocket* _socket = nullptr;
    int _port = 0;

    const int _connectionTimeout = 4000;
    QTimer _timeoutTimer;

public:
    explicit TCPServerMonitorPrivate(TCPServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~TCPServerMonitorPrivate() = default;
};

net::TCPServerMonitor::TCPServerMonitor(const QString &address, int port, QObject *parent)
    : ServerMonitor(address, parent)
    , d_ptr(new TCPServerMonitorPrivate(this))
{
    Q_D(TCPServerMonitor);

    d->_timeoutTimer.setSingleShot(true);
    d->_timeoutTimer.setInterval(d->_connectionTimeout);

    d->_socket = new QTcpSocket(this);
    d->_port = port;

    connect(d->_socket, &QTcpSocket::connected, this, [this]()
    {
        Q_D(TCPServerMonitor);

        d->_timeoutTimer.stop();
        d->_socket->abort();

        emit finished(true);
    });

    connect(d->_socket, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
        this, [this](QAbstractSocket::SocketError)
    {
        Q_D(TCPServerMonitor);

        if (!d->_timeoutTimer.isActive())
            return;

        d->_timeoutTimer.stop();
        d->_socket->close();

        emit finished(false);
    });

    connect(&d->_timeoutTimer, &QTimer::timeout, this, [this]()
    {
        Q_D(TCPServerMonitor);

        if (d->_socket->state() != QAbstractSocket::SocketState::ConnectedState)
        {
            d->_timeoutTimer.stop();
            d->_socket->abort();
            emit finished(false);
        }
    });
}

net::TCPServerMonitor::~TCPServerMonitor() = default;

void net::TCPServerMonitor::checkServer()
{
    Q_D(TCPServerMonitor);

    if (d->_timeoutTimer.isActive() && d->_socket->state() == QAbstractSocket::SocketState::ConnectingState)
        emit finished(false);

    d->_timeoutTimer.stop();
    d->_socket->abort();

    d->_timeoutTimer.start();
    d->_socket->connectToHost(address(), d->_port);
}
