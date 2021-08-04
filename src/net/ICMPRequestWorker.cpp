#include "ICMPRequestWorker.h"

#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"

#include <chrono>


class ICMPRequestWorkerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ICMPRequestWorker)
    Q_DISABLE_COPY(ICMPRequestWorkerPrivate)
    ICMPRequestWorker *q_ptr = nullptr;

    ICMPMessage _message;
    int _timeout;

public:
    explicit ICMPRequestWorkerPrivate(ICMPRequestWorker *q)
        : q_ptr(q)
    {
    }

    ~ICMPRequestWorkerPrivate() = default;
};

ICMPRequestWorker::ICMPRequestWorker(ICMPMessage message, int timeout /*= 1000*/, QObject *parent /*= nullptr*/)
    : QObject(parent)
    , d_ptr(new ICMPRequestWorkerPrivate(this))
{
    Q_D(ICMPRequestWorker);

    d->_message = message;
    d->_timeout = timeout;
}

ICMPRequestWorker::~ICMPRequestWorker()
{
}

void ICMPRequestWorker::send_request()
{
    Q_D(ICMPRequestWorker);

    auto timeStart = std::chrono::high_resolution_clock::now();

    DWORD dwRetVal = IcmpSendEcho(
        d->_message.hIcmpFile,
        d->_message.ipaddr,
        d->_message.SendData,
        sizeof(d->_message.SendData),
        NULL,
        d->_message.ReplyBuffer,
        d->_message.ReplySize,
        d->_timeout);

    auto timeEnd = std::chrono::high_resolution_clock::now();

    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();

    free(d->_message.ReplyBuffer);


    emit ready(dwRetVal != 0, latency);
}
