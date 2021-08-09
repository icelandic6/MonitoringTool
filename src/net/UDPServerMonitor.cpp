#include "UDPServerMonitor.h"
#include "UDPListener.h"

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

    void showError(const QString &message)
    {
        Q_Q(UDPServerMonitor);

        if (!::socket_errored)
        {
            QMessageBox::warning(nullptr, QString("Monitoring Tool"),
                QString("UDP socket error for address %1:%2\n%3").arg(q->address()).arg(_port).arg(message),
                QMessageBox::Ok, QMessageBox::Ok);
        }
        ::socket_errored = true;

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
        d->showError(QString("Could not startup WSA. Error code: %1").arg(WSAGetLastError()));

    auto thread = new QThread();
    auto listener = new UDPListener(d->_port);
    listener->moveToThread(thread);

    connect(thread, &QThread::started, listener, &UDPListener::start);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    connect(listener, &UDPListener::ready, thread, &QThread::quit);
    connect(listener, &UDPListener::ready, listener, &QObject::deleteLater);
    connect(listener, &UDPListener::ready, this, [this](bool success)
    {
        emit finished(success);
    });

    thread->start();

    d->_port = port;
}

net::UDPServerMonitor::~UDPServerMonitor() = default;


void net::UDPServerMonitor::checkServer()
{
    qDebug() << "\n==== Running UDP server check";

    Q_D(UDPServerMonitor);

    qDebug() << QString("==== UDP CHECK: CONNECTING TO %1:%2").arg(address()).arg(d->_port);

    int socksend;
    const char *hello = "ping";
    struct sockaddr_in addrsend;

    if ((socksend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
        d->showError(QString("Could not create sending socket"));
        return;
    }

    memset(&addrsend, 0, sizeof(addrsend));

    addrsend.sin_family = AF_INET;
    addrsend.sin_addr.s_addr = inet_addr(address().toUtf8());
    addrsend.sin_port = htons(d->_port);

    int len = sizeof(addrsend);

    int res = 0;
    if ((res = sendto(socksend, hello, strlen(hello), 0, (const struct sockaddr *)&addrsend, len)) == SOCKET_ERROR)
    {
        d->showError(QString("Error sending message [%1]").arg(WSAGetLastError()));
        return;
    }

    qDebug() << "UDP Socket sent: " << res << " bytes";
}
