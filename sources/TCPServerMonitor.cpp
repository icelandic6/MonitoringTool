#include "TCPServerMonitor.h"

#include <QtNetwork/QTcpSocket>
#include <QTimer>

class TCPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(TCPServerMonitor)
    Q_DISABLE_COPY(TCPServerMonitorPrivate)
    TCPServerMonitor *q_ptr = nullptr;

    QAbstractSocket* _socket;

    int _connectionTimeout = 3000;

public:
    explicit TCPServerMonitorPrivate(TCPServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~TCPServerMonitorPrivate()
    {
    }

    void onError(QAbstractSocket::SocketError socketError)
    {
        qDebug() << QString("==== TCP CHECK: ERROR [%1]").arg(socketError);
    }
};

TCPServerMonitor::TCPServerMonitor(QString name, QString hostAddress, int port, QObject *parent)
    : ServerMonitor(name, hostAddress, port, parent)
    , d_ptr(new TCPServerMonitorPrivate(this))
{
    qDebug() << QString("====== TCPServerMonitor: host address = [%1]").arg(hostAddress);

    Q_D(TCPServerMonitor);
    d->_socket = new QTcpSocket(this);

    qRegisterMetaType<QTcpSocket::SocketError>("SocketError");

    connect(d->_socket, &QTcpSocket::stateChanged, this, [this](QAbstractSocket::SocketState socketState)
    {
        qDebug() << QString("==== TCP CHECK: STATE %1").arg(socketState);
    });

    connect(d->_socket, &QTcpSocket::connected, this, [this]()
    {
        qDebug() << QString("==== TCP CHECK: CONNECTED");
        emit succeeded();
    });

    connect(d->_socket, SIGNAL(error(SocketError)),
        this, SLOT(onError(QAbstractSocket::SocketError)));
}

// TCPServerMonitor::TCPServerMonitor(QString name, QString hostAddress, int port, QObject *parent)
// {
// }

TCPServerMonitor::~TCPServerMonitor() = default;

void TCPServerMonitor::checkServer()
{
    Q_D(TCPServerMonitor);

    qDebug() << "==== Running TCP server check";
    QTimer::singleShot(d->_connectionTimeout, this, [this]()
    {
        Q_D(TCPServerMonitor);

        if (d->_socket->state() != QAbstractSocket::SocketState::ConnectedState)
            d->_socket->close();

        emit failed();
    });

    qDebug() << QString("==== TCP CHECK: CONNECTING TO %1:%2").arg(hostAddress()).arg(hostPort());
    d->_socket->connectToHost(hostAddress(), hostPort());
}

void TCPServerMonitor::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << QString("==== TCP CHECK: ERROR [%1]").arg(socketError);

    Q_D(TCPServerMonitor);

    d->_socket->close();
    emit failed();
}
