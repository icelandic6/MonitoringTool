#include "UDPListener.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"

#include <QDebug>


class net::UDPListenerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(UDPListener)
    Q_DISABLE_COPY(UDPListenerPrivate)
    UDPListener *q_ptr = nullptr;

    const int _timeout = 3000;
    int _port;

public:
    explicit UDPListenerPrivate(UDPListener *q)
        : q_ptr(q)
    {
    }

    ~UDPListenerPrivate() = default;
};

net::UDPListener::UDPListener(int port, QObject *parent /*= nullptr*/)
    : QObject(parent)
    , d_ptr(new UDPListenerPrivate(this))
{
    Q_D(UDPListener);

    d->_port = port;
}

net::UDPListener::~UDPListener() = default;

void net::UDPListener::start()
{
    Q_D(UDPListener);


    int sockrecv;
    char recvbuf[1024];
    struct sockaddr_in addrrecv;
    int lenrecv = sizeof(addrrecv);

    if ((sockrecv = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == INVALID_SOCKET)
    {
        qDebug() << "error: create socket: " << WSAGetLastError();
        return;
    }

//     DWORD timeoutmilsec = 3000;
//     setsockopt(sockrecv, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeoutmilsec, sizeof(timeoutmilsec));

    memset(&addrrecv, 0, sizeof(addrrecv));

    addrrecv.sin_family = AF_INET;
    addrrecv.sin_addr.s_addr = INADDR_ANY;
    addrrecv.sin_port = htons(d->_port);

    int res = -1;
    if ((res = bind(sockrecv, (const struct sockaddr *)&addrrecv, sizeof(addrrecv))) < 0)
    {
        qDebug() << "error: bind: " << WSAGetLastError();
        return;
    }

    res = 0;
//     if (res = recv(sockrecv, recvbuf, sizeof(recvbuf), 0) < 0)
//         qDebug() << "error: recv: " << WSAGetLastError();

    if (res = recvfrom(sockrecv, recvbuf, sizeof(recvbuf), 0, (sockaddr*)&addrrecv, &lenrecv) < 0)
    {
        qDebug() << "error: recv: " << WSAGetLastError();
        return;
    }

    qDebug() << "UDP socket recved: " << res << " bytes";
}
