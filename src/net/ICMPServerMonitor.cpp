#include "ICMPRequestWorker.h"
#include "ICMPServerMonitor.h"

#include <QTimer>
#include <QThread>
#include <QHostInfo>

class ICMPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ICMPServerMonitor)
    Q_DISABLE_COPY(ICMPServerMonitorPrivate)
    ICMPServerMonitor *q_ptr = nullptr;

    QHostAddress _ipv4Address;
    bool _useResolving = false;
    ushort _ipv4AddressUsages = 0;
    const ushort _ipv4AddressUsageMax = 60;

public:
    explicit ICMPServerMonitorPrivate(ICMPServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~ICMPServerMonitorPrivate() = default;

    void hostLookedUp(QHostInfo info)
    {
        _ipv4Address = info.addresses().first();
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

ICMPServerMonitor::ICMPServerMonitor(const QString &address, QObject *parent)
    : ServerMonitor(address, parent)
    , d_ptr(new ICMPServerMonitorPrivate(this))
{
    Q_D(ICMPServerMonitor);

    qDebug() << QString("\n====== ICMPServerMonitor: host address = [%1]\n").arg(address);

    d->_ipv4Address = QHostAddress(address);

    if (d->_ipv4Address.isNull())
    {
        d->_useResolving = true;
        QHostInfo::lookupHost(address, d, &ICMPServerMonitorPrivate::hostLookedUp);
    }
}

ICMPServerMonitor::~ICMPServerMonitor() = default;

void ICMPServerMonitor::checkServer()
{
    Q_D(ICMPServerMonitor);

    qDebug() << "\n====== Running ICMP server check ======";

    if (d->_ipv4Address.isNull())
    {
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
