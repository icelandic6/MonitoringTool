#include "ICMPRequestWorker.h"
#include "ICMPServerMonitor.h"
#include "core/Logger.h"

#include <QThread>
#include <QHostInfo>

class net::ICMPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ICMPServerMonitor)
    Q_DISABLE_COPY(ICMPServerMonitorPrivate)
    ICMPServerMonitor *q_ptr = nullptr;

    QHostAddress _ipv4Address;
    bool _useResolving = false;
    ushort _ipv4AddressUsages = 0;
    const ushort _ipv4AddressUsageMax = 60;
    bool _resolveFailed = false;

public:
    explicit ICMPServerMonitorPrivate(ICMPServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~ICMPServerMonitorPrivate() = default;

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
            Q_Q(ICMPServerMonitor);
            QHostInfo::lookupHost(q->address(), this, &ICMPServerMonitorPrivate::hostLookedUp);
            _ipv4AddressUsages = 0;
        }
        else
            _ipv4AddressUsages++;
    }
};

net::ICMPServerMonitor::ICMPServerMonitor(const QString &address, QObject *parent)
    : ServerMonitor(address, parent)
    , d_ptr(new ICMPServerMonitorPrivate(this))
{
    Q_D(ICMPServerMonitor);

    d->_ipv4Address = QHostAddress(address);

    if (d->_ipv4Address.isNull())
    {
        d->_useResolving = true;
        QHostInfo::lookupHost(address, d, &ICMPServerMonitorPrivate::hostLookedUp);
    }
}

net::ICMPServerMonitor::~ICMPServerMonitor() = default;

void net::ICMPServerMonitor::checkServer()
{
    Q_D(ICMPServerMonitor);

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

    auto _requestThread = new QThread();

    auto _requestWorker = new ICMPRequestWorker(d->_ipv4Address.toString());
    _requestWorker->moveToThread(_requestThread);

    connect(_requestThread, &QThread::started, _requestWorker, &ICMPRequestWorker::send_request);
    connect(_requestThread, &QThread::finished, _requestThread, &QObject::deleteLater);

    connect(_requestWorker, &ICMPRequestWorker::ready, _requestThread, &QThread::quit);
    connect(_requestWorker, &ICMPRequestWorker::ready, _requestWorker, &QObject::deleteLater);
    connect(_requestWorker, &ICMPRequestWorker::ready, this, [this](bool success, int latency)
    {
        emit finished(success, latency);
    });

    _requestThread->start();

    if (d->_useResolving)
        d->checkIPv4AddressUsage();
}
