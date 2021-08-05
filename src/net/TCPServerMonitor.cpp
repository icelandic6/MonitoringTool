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

    const int _connectionTimeout = 8000;
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

    qRegisterMetaType<QTcpSocket::SocketError>("SocketError");

    connect(d->_socket, &QTcpSocket::connected, this, [this]()
    {
        Q_D(TCPServerMonitor);

        d->_timeoutTimer.stop();
        d->_socket->abort();

        emit finished(true);
    });

    connect(d->_socket, SIGNAL(error(SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));

    connect(&d->_timeoutTimer, &QTimer::timeout, this, [this]()
    {
        Q_D(TCPServerMonitor);

        if (d->_socket->state() != QAbstractSocket::SocketState::ConnectedState)
        {
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

    d->_socket->connectToHost(address(), d->_port);
    d->_timeoutTimer.start();
}

void net::TCPServerMonitor::onError(QAbstractSocket::SocketError socketError)
{
    Q_D(TCPServerMonitor);

    if (!d->_timeoutTimer.isActive())
        return;

    d->_timeoutTimer.stop();
    d->_socket->close();

    emit finished(false);
}
