#include "UDPServerMonitor.h"
#include "UDPWorker.h"

#include <QtNetwork/QUdpSocket>
#include <QMessageBox>
#include <QThread>

#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"

static bool socket_errored = false;

class net::UDPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(UDPServerMonitor)
    Q_DISABLE_COPY(UDPServerMonitorPrivate)
    UDPServerMonitor *q_ptr = nullptr;

    QAbstractSocket* _socket = nullptr;
    QAbstractSocket* _socketIn = nullptr;
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

    d->_port = port;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        qDebug() << QString("UDP check: could not startup WSA. Error code: %1").arg(WSAGetLastError());

    d->_port = port;
}

net::UDPServerMonitor::~UDPServerMonitor() = default;


void net::UDPServerMonitor::checkServer()
{
    qDebug() << "\n==== Running UDP server check";

    Q_D(UDPServerMonitor);

    qDebug() << QString("==== UDP CHECK: CONNECTING TO %1:%2").arg(address()).arg(d->_port);

    auto thread = new QThread();
    auto worker = new UDPWorker(address(), d->_port);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &UDPWorker::start);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    connect(worker, &UDPWorker::ready, thread, &QThread::quit);
    connect(worker, &UDPWorker::ready, worker, &QObject::deleteLater);
    connect(worker, &UDPWorker::ready, this, [this](bool success)
    {
        emit finished(success);
    });

    thread->start();
}
