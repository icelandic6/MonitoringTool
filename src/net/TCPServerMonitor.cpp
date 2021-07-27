#include "TCPServerMonitor.h"

#include <QtNetwork/QTcpSocket>
#include <QTimer>

class TCPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(TCPServerMonitor)
    Q_DISABLE_COPY(TCPServerMonitorPrivate)
    TCPServerMonitor *q_ptr = nullptr;

    QAbstractSocket* _socket = nullptr;

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

TCPServerMonitor::TCPServerMonitor(QString name, QString hostAddress, int port, QObject *parent)
    : ServerMonitor(name, hostAddress, port, parent)
    , d_ptr(new TCPServerMonitorPrivate(this))
{
    qDebug() << QString("====== TCPServerMonitor: host address = [%1]").arg(hostAddress);

    Q_D(TCPServerMonitor);
    d->_timeoutTimer.setSingleShot(true);
    d->_timeoutTimer.setInterval(d->_connectionTimeout);

    d->_socket = new QTcpSocket(this);

    qRegisterMetaType<QTcpSocket::SocketError>("SocketError");

    connect(d->_socket, &QTcpSocket::stateChanged, this, [this](QAbstractSocket::SocketState socketState)
    {
        qDebug() << QString("==== TCP CHECK: STATE %1").arg(socketState);
    });

    connect(d->_socket, &QTcpSocket::connected, this, [this]()
    {
        Q_D(TCPServerMonitor);

        qDebug() << QString("==== TCP CHECK: CONNECTED");
        d->_lastAttempt = true;
        d->_timeoutTimer.stop();

        d->_socket->close();
        emit succeeded();
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
            emit failed();
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

    qDebug() << QString("==== TCP CHECK: CONNECTING TO %1:%2").arg(hostAddress()).arg(hostPort());
    d->_socket->connectToHost(hostAddress(), hostPort());
    d->_timeoutTimer.start();
}

void TCPServerMonitor::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << QString("==== TCP CHECK: ERROR [%1]").arg(socketError);

    Q_D(TCPServerMonitor);

    d->_timeoutTimer.stop();
    d->_lastAttempt = false;
    d->_socket->close();

    emit failed();
}
