#include "UDPServerMonitor.h"
#include "UDPWorker.h"

#include "core/Logger.h"

#include <QThread>
#include <QHostAddress>
#include <QHostInfo>

#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"

class net::UDPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(UDPServerMonitor)
    Q_DISABLE_COPY(UDPServerMonitorPrivate)
    UDPServerMonitor *q_ptr = nullptr;

    int _port = 0;

    QHostAddress _ipv4Address;
    bool _useResolving = false;
    ushort _ipv4AddressUsages = 0;
    const ushort _ipv4AddressUsageMax = 60;
    bool _resolveFailed = false;

public:
    explicit UDPServerMonitorPrivate(UDPServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~UDPServerMonitorPrivate() = default;

    void hostLookedUp(QHostInfo info)
    {
        auto addresses = info.addresses();
        if (!addresses.isEmpty())
            _ipv4Address = addresses.first();
    }

    void checkIPv4AddressUsage()
    {
        if (_ipv4AddressUsages >= _ipv4AddressUsageMax)
        {
            Q_Q(UDPServerMonitor);
            QHostInfo::lookupHost(q->address(), this, &UDPServerMonitorPrivate::hostLookedUp);
            _ipv4AddressUsages = 0;
        }
        else
            _ipv4AddressUsages++;
    }
};

net::UDPServerMonitor::UDPServerMonitor(const QString &address, int port, QObject *parent)
    : ServerMonitor(address, parent)
    , d_ptr(new UDPServerMonitorPrivate(this))
{
    Q_D(UDPServerMonitor);

    d->_port = port;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        core::Logger::instance()->addLog(
            QString("UDP check: could not startup WSA. Error code: %1").arg(WSAGetLastError()));

    d->_port = port;

    d->_ipv4Address = QHostAddress(address);

    if (d->_ipv4Address.isNull())
    {
        d->_useResolving = true;
        QHostInfo::lookupHost(address, d, &UDPServerMonitorPrivate::hostLookedUp);
    }
}

net::UDPServerMonitor::~UDPServerMonitor() = default;


void net::UDPServerMonitor::checkServer()
{
    qDebug() << "\n==== Running UDP server check";

    Q_D(UDPServerMonitor);

    qDebug() << QString("==== UDP CHECK: CONNECTING TO %1:%2").arg(address()).arg(d->_port);

    if (d->_ipv4Address.isNull())
    {
        if (!d->_resolveFailed)
        {
            core::Logger::instance()->addLog(QString("Couldn't resolve IP address [%1]").arg(address()));
            d->_resolveFailed = true;
        }
        emit finished(false);
        return;
    }

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
