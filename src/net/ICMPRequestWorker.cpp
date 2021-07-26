#include "ICMPRequestWorker.h"

#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"

#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QThread>


class ICMPRequestWorkerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ICMPRequestWorker)
    Q_DISABLE_COPY(ICMPRequestWorkerPrivate)
    ICMPRequestWorker *q_ptr = nullptr;

public:
    explicit ICMPRequestWorkerPrivate(ICMPRequestWorker *q)
        : q_ptr(q)
    {
    }

    ~ICMPRequestWorkerPrivate()
    {
    }
};

ICMPRequestWorker::ICMPRequestWorker(QObject *parent)
    : QObject(parent)
    , d_ptr(new ICMPRequestWorkerPrivate(this))
{
    qDebug() << QString("====== ICMPRequestWorker: created\n");
    qDebug() << QString("====== ICMPRequestWorker: created in thread [%1]")
        .arg(int(QThread::currentThreadId())) << QThread::currentThread();
    Q_D(ICMPRequestWorker);
}

// ICMPRequestWorker::~ICMPRequestWorker() = default;
ICMPRequestWorker::~ICMPRequestWorker()
{
    qDebug() << "====== ICMPRequestWorker: destroyed";
}

void ICMPRequestWorker::send_request(ICMPMessage &message, int timeout)
{
    Q_D(ICMPRequestWorker);
    qDebug() << QString("====== ICMPRequestWorker: send_request");
    qDebug() << QString("====== ICMPServerMonitor: send_request in thread [%1]")
        .arg(int(QThread::currentThreadId())) << QThread::currentThread();

    DWORD dwRetVal = IcmpSendEcho(
        message.hIcmpFile,
        message.ipaddr,
        message.SendData,
        sizeof(message.SendData),
        NULL,
        message.ReplyBuffer,
        message.ReplySize,
        timeout);

    free(message.ReplyBuffer);

    emit ready(dwRetVal != 0);

//     if (dwRetVal != 0) {
//         PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)icmpMessage.ReplyBuffer;
//         struct in_addr ReplyAddr;
//         ReplyAddr.S_un.S_addr = pEchoReply->Address;
// 
//         strMessage += QString("Sent ICMP message to %1\n").arg(hostAddress());
// 
//         if (icmpMessage.dwRetVal > 1) {
//             strMessage += "Received " + QString::number(icmpMessage.dwRetVal) + " ICMP message responses \n";
//             strMessage += "Information from the first response: ";
//         }
//         else {
//             strMessage += "Received " + QString::number(icmpMessage.dwRetVal) + " ICMP message response \n";
//             strMessage += "Information from the first response: ";
//         }
//         strMessage += "Received from ";
//         strMessage += inet_ntoa(ReplyAddr);
//         strMessage += "\n";
//         strMessage += "Status = " + pEchoReply->Status;
//         strMessage += "Roundtrip time = " + QString::number(pEchoReply->RoundTripTime) + " milliseconds \n";
//         strMessage += "Roundtrip time = " + QString::number(pEchoReply->RoundTripTime) + " milliseconds \n";
//     }
//     else {
//         strMessage += "Call to IcmpSendEcho failed.\n";
//         strMessage += "IcmpSendEcho returned error: ";
//         strMessage += QString::number(GetLastError());
//     }

}
