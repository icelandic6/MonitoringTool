#include "UDPWorker.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"

#include <QDebug>

class net::UDPWorkerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(UDPWorker)
    Q_DISABLE_COPY(UDPWorkerPrivate)
    UDPWorker *q_ptr = nullptr;

    const DWORD _timeout = 3000;
    QString _address;
    int _port;

public:
    explicit UDPWorkerPrivate(UDPWorker *q)
        : q_ptr(q)
    {
    }

    ~UDPWorkerPrivate() = default;
};

net::UDPWorker::UDPWorker(const QString &address, int port, QObject *parent /*= nullptr*/)
    : QObject(parent)
    , d_ptr(new UDPWorkerPrivate(this))
{
    Q_D(UDPWorker);

    d->_address = address;
    d->_port = port;
}

net::UDPWorker::~UDPWorker() = default;

void net::UDPWorker::start()
{
    Q_D(UDPWorker);

    int socksend;
    const char *hello = "ping";
    struct sockaddr_in addrsend;
    char recvbuf[256];

    if ((socksend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
        qDebug() << "Error: create socket [" << WSAGetLastError() << "]";
        return;
    }

    memset(&addrsend, 0, sizeof(addrsend));

    addrsend.sin_family = AF_INET;
    addrsend.sin_addr.s_addr = inet_addr(d->_address.toUtf8());
    addrsend.sin_port = htons(d->_port);

    int res = -1;
    if ((res = ::connect(socksend, (const struct sockaddr *)&addrsend, sizeof(addrsend))) == SOCKET_ERROR)
    {
        qDebug() << "Error: connect [" << WSAGetLastError() << "]";
        emit ready(false);
        return;
    }

    if ((res = send(socksend, hello, strlen(hello), 0)) == SOCKET_ERROR)
    {
        qDebug() << "Error: send message [" << WSAGetLastError() << "]";
        emit ready(false);
        return;
    }

    qDebug() << "UDP Socket sent: " << res << " bytes";

    if ((res = setsockopt(socksend,
        SOL_SOCKET, SO_RCVTIMEO,
        (char *)&d->_timeout, sizeof(d->_timeout))) == SOCKET_ERROR)
    {
        qDebug() << "Error: setsockopt [" << WSAGetLastError() << "]";
        emit ready(false);
        return;
    }

    if (res = recv(socksend, recvbuf, sizeof(recvbuf), 0) < 0)
    {
        qDebug() << "Error: receive message [" << WSAGetLastError() << "]";
        emit ready(false);
    }
    else
        emit ready(true);

    closesocket(socksend);
}
