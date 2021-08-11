#include "ICMPWorker.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"

#include <chrono>

class net::ICMPWorkerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ICMPWorker)
    Q_DISABLE_COPY(ICMPWorkerPrivate)
    ICMPWorker *q_ptr = nullptr;

    const int _timeout = 3000;
    QString _addr;

public:
    explicit ICMPWorkerPrivate(ICMPWorker *q)
        : q_ptr(q)
    {
    }

    ~ICMPWorkerPrivate() = default;
};

net::ICMPWorker::ICMPWorker(const QString &addr, QObject *parent /*= nullptr*/)
    : QObject(parent)
    , d_ptr(new ICMPWorkerPrivate(this))
{
    Q_D(ICMPWorker);
    d->_addr = addr;
}

net::ICMPWorker::~ICMPWorker() = default;

void net::ICMPWorker::send_request()
{
    Q_D(ICMPWorker);

    auto hIcmpFile = IcmpCreateFile();
    unsigned long ipaddr = inet_addr(d->_addr.toStdString().c_str());
    char SendData[32] = "Data Buffer";
    DWORD ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    LPVOID ReplyBuffer = (VOID*)malloc(ReplySize);

    auto timeStart = std::chrono::high_resolution_clock::now();

    DWORD dwRetVal = IcmpSendEcho(
        hIcmpFile,
        ipaddr,
        SendData,
        sizeof(SendData),
        NULL,
        ReplyBuffer,
        ReplySize,
        d->_timeout);

    auto timeEnd = std::chrono::high_resolution_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();

    IcmpCloseHandle(hIcmpFile);

    if (dwRetVal == 0)
    {
        free(ReplyBuffer);
        emit ready(false, 0);
        return;
    }

    auto success = ((PICMP_ECHO_REPLY)ReplyBuffer)->Status == IP_SUCCESS;

    free(ReplyBuffer);
    emit ready(success, success ? latency : 0);
}
