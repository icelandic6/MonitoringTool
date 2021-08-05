#include "ICMPRequestWorker.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"

#include <chrono>

class ICMPRequestWorkerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ICMPRequestWorker)
    Q_DISABLE_COPY(ICMPRequestWorkerPrivate)
    ICMPRequestWorker *q_ptr = nullptr;

    const int _timeout = 3000;
    QString _addr;

public:
    explicit ICMPRequestWorkerPrivate(ICMPRequestWorker *q)
        : q_ptr(q)
    {
    }

    ~ICMPRequestWorkerPrivate() = default;
};

ICMPRequestWorker::ICMPRequestWorker(const QString &addr, QObject *parent /*= nullptr*/)
    : QObject(parent)
    , d_ptr(new ICMPRequestWorkerPrivate(this))
{
    Q_D(ICMPRequestWorker);
    d->_addr = addr;
}

ICMPRequestWorker::~ICMPRequestWorker() = default;

void ICMPRequestWorker::send_request()
{
    Q_D(ICMPRequestWorker);

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

    free(ReplyBuffer);

    IcmpCloseHandle(hIcmpFile);

    emit ready(dwRetVal != 0, dwRetVal == 0 ? 0 : latency);
}
