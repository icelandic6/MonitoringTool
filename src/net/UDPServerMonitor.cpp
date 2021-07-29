#include "UDPServerMonitor.h"

#include <QtNetwork/QUdpSocket>
#include <QTimer>

class UDPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(UDPServerMonitor)
    Q_DISABLE_COPY(UDPServerMonitorPrivate)
    UDPServerMonitor *q_ptr = nullptr;

    QAbstractSocket* _socket = nullptr;

    int _connectionTimeout = 3000;
    QTimer _timeoutTimer;

    bool _lastAttempt = false;

public:
    explicit UDPServerMonitorPrivate(UDPServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~UDPServerMonitorPrivate()
    {
    }
};

UDPServerMonitor::UDPServerMonitor(QString name, QString hostAddress, int port, QObject *parent)
    : ServerMonitor(name, hostAddress, port, parent)
    , d_ptr(new UDPServerMonitorPrivate(this))
{
    qDebug() << QString("====== UDPServerMonitor: host address = [%1]").arg(hostAddress);

    Q_D(UDPServerMonitor);
    d->_timeoutTimer.setSingleShot(true);
    d->_timeoutTimer.setInterval(d->_connectionTimeout);

    d->_socket = new QUdpSocket(this);

    qRegisterMetaType<QUdpSocket::SocketError>("SocketError");

    connect(d->_socket, &QUdpSocket::stateChanged, this, [this](QAbstractSocket::SocketState socketState)
    {
        qDebug() << QString("==== UDP CHECK: STATE %1").arg(socketState);
    });

    connect(d->_socket, &QUdpSocket::connected, this, [this]()
    {
        Q_D(UDPServerMonitor);

        qDebug() << QString("==== UDP CHECK: CONNECTED");

        d->_socket->close();
        emit finished(true);
    });

    connect(d->_socket, SIGNAL(error(SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
}

UDPServerMonitor::~UDPServerMonitor() = default;

void UDPServerMonitor::checkServer()
{
    Q_D(UDPServerMonitor);

    qDebug() << "==== Running UDP server check";
    d->_socket->close();

    qDebug() << QString("==== UDP CHECK: CONNECTING TO %1:%2").arg(hostAddress()).arg(hostPort());
    d->_socket->connectToHost(hostAddress(), hostPort());
}

void UDPServerMonitor::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << QString("==== UDP CHECK: ERROR [%1]").arg(socketError);

    Q_D(UDPServerMonitor);

    d->_socket->close();
    emit finished(false);
}
