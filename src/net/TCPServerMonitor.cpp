#include "TCPServerMonitor.h"

#include <QtNetwork/QTcpSocket>
#include <QTimer>

class TCPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(TCPServerMonitor)
    Q_DISABLE_COPY(TCPServerMonitorPrivate)
    TCPServerMonitor *q_ptr = nullptr;

    QAbstractSocket* _socket = nullptr;
    int _port = 0;

    int _connectionTimeout = 3000;
    QTimer _timeoutTimer;

    bool _lastAttempt = false;

public:
    explicit TCPServerMonitorPrivate(TCPServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~TCPServerMonitorPrivate()
    {
    }
};

TCPServerMonitor::TCPServerMonitor(const QString &address, int port, QObject *parent)
    : ServerMonitor(address, parent)
    , d_ptr(new TCPServerMonitorPrivate(this))
{
    qDebug() << QString("====== TCPServerMonitor: host address = [%1]").arg(address);

    Q_D(TCPServerMonitor);
    d->_timeoutTimer.setSingleShot(true);
    d->_timeoutTimer.setInterval(d->_connectionTimeout);

    d->_socket = new QTcpSocket(this);
    d->_port = port;

    qRegisterMetaType<QTcpSocket::SocketError>("SocketError");

//     connect(d->_socket, &QTcpSocket::stateChanged, this, [this](QAbstractSocket::SocketState socketState)
//     {
//         qDebug() << QString("==== TCP CHECK: STATE %1").arg(socketState);
//     });

    connect(d->_socket, &QTcpSocket::connected, this, [this]()
    {
        Q_D(TCPServerMonitor);

        qDebug() << QString("==== TCP CHECK: CONNECTED");
        d->_lastAttempt = true;
        d->_timeoutTimer.stop();

        d->_socket->close();
        emit finished(true);
    });

    connect(d->_socket, SIGNAL(error(SocketError)),
        this, SLOT(onError(QAbstractSocket::SocketError)));

    connect(&d->_timeoutTimer, &QTimer::timeout, this, [this]()
    {
        Q_D(TCPServerMonitor);

        qDebug() << QString("==== TCP CHECK: TIMEOUT");

        if (d->_socket->state() != QAbstractSocket::SocketState::ConnectedState)
        {
            qDebug() << QString("==== TCP CHECK: TIMEOUT STATE = %1").arg(d->_socket->state());
            d->_socket->close();
            emit finished(false);
        }
    });
}

TCPServerMonitor::~TCPServerMonitor() = default;

void TCPServerMonitor::checkServer()
{
    Q_D(TCPServerMonitor);

    qDebug() << "==== Running TCP server check";
    d->_timeoutTimer.stop();
    d->_lastAttempt = false;
    d->_socket->close();

    qDebug() << QString("==== TCP CHECK: CONNECTING TO %1:%2").arg(address()).arg(d->_port);
    d->_socket->connectToHost(address(), d->_port);
    d->_timeoutTimer.start();
}

void TCPServerMonitor::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << QString("==== TCP CHECK: ERROR [%1]").arg(socketError);

    Q_D(TCPServerMonitor);

    if (!d->_timeoutTimer.isActive())
        return;

    d->_timeoutTimer.stop();
    d->_lastAttempt = false;
    d->_socket->close();

    emit finished(false);
}
