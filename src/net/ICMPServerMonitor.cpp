#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#include "ICMPMessage.h"
#include "ICMPRequestWorker.h"
#include "ICMPServerMonitor.h"

#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QThread>


class ICMPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ICMPServerMonitor)
    Q_DISABLE_COPY(ICMPServerMonitorPrivate)
    ICMPServerMonitor *q_ptr = nullptr;

public:
    explicit ICMPServerMonitorPrivate(ICMPServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~ICMPServerMonitorPrivate()
    {
    }
};

ICMPServerMonitor::ICMPServerMonitor(QString name, QString hostAddress, QObject *parent)
    : ServerMonitor(name, hostAddress, 0, parent)
    , d_ptr(new ICMPServerMonitorPrivate(this))
{
}

ICMPServerMonitor::~ICMPServerMonitor() = default;

void ICMPServerMonitor::checkServer()
{
    Q_D(ICMPServerMonitor);

    qDebug() << "\n\n====== Running ICMP server check ======\n";

    ICMPMessage message;

    message.ipaddr = inet_addr(hostAddress().toStdString().c_str());
    message.hIcmpFile = IcmpCreateFile();

    message.ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(message.SendData);
    message.ReplyBuffer = (VOID*)malloc(message.ReplySize);


    auto _requestThread = new QThread();

    auto _requestWorker = new ICMPRequestWorker(message, 6000);
    _requestWorker->moveToThread(_requestThread);

    connect(_requestThread, &QThread::started, _requestWorker, &ICMPRequestWorker::send_request);
    connect(_requestThread, &QThread::finished, _requestThread, &QObject::deleteLater);

    connect(_requestWorker, &ICMPRequestWorker::ready, _requestThread, &QThread::quit);
    connect(_requestWorker, &ICMPRequestWorker::ready, _requestWorker, &QObject::deleteLater);

    connect(_requestWorker, &ICMPRequestWorker::ready, this, [this](bool success)
    {
        if (success)
            emit succeeded();
        else
            emit failed();
    });

    _requestThread->start();
}
