#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#include "ICMPMessage.h"
#include "ICMPRequestWorker.h"
#include "ICMPServerMonitor.h"

// #include "winsock2.h"
// #include "iphlpapi.h"
// #include "icmpapi.h"

#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QThread>


class ICMPServerMonitorPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ICMPServerMonitor)
    Q_DISABLE_COPY(ICMPServerMonitorPrivate)
    ICMPServerMonitor *q_ptr = nullptr;

    QThread _thread;
    ICMPRequestWorker *_requestWorker = nullptr;

public:
    explicit ICMPServerMonitorPrivate(ICMPServerMonitor *q)
        : q_ptr(q)
    {
    }

    ~ICMPServerMonitorPrivate()
    {
    }
};

ICMPServerMonitor::ICMPServerMonitor(QString name, QString hostAddress, int port, QObject *parent)
    : ServerMonitor(name, hostAddress, port, parent)
    , d_ptr(new ICMPServerMonitorPrivate(this))
{
    qDebug() << QString("====== ICMPServerMonitor: creating with host address = [%1]").arg(hostAddress);
    qDebug() << QString("====== ICMPServerMonitor: creating thread ") << QThread::currentThread();

    Q_D(ICMPServerMonitor);
    
    d->_thread.setObjectName("ICMP worker thread");
    qDebug() << QString("====== ICMPServerMonitor: worker thread is ") << &d->_thread << "\n";

    d->_requestWorker = new ICMPRequestWorker();
    d->_requestWorker->moveToThread(&d->_thread);

    connect(&d->_thread, &QThread::finished, d->_requestWorker, &QObject::deleteLater);

    connect(d->_requestWorker, &ICMPRequestWorker::ready, this, [this](bool success)
    {
        qDebug() << QString("====== ICMPServerMonitor: ready thread [%1]").arg(int(QThread::currentThreadId())) << QThread::currentThread();
        qDebug() << QString("====== ICMPServerMonitor: ICMP request ready [%1]").arg(success);

        if (success)
            emit succeeded();
        else
            emit failed();
    });

    d->_thread.start();
}

ICMPServerMonitor::~ICMPServerMonitor() = default;

void ICMPServerMonitor::checkServer()
{
    Q_D(ICMPServerMonitor);

    qDebug() << "==== Running ICMP server check";
    qDebug() << QString("====== ICMPServerMonitor: starting thread [%1]")
        .arg(int(QThread::currentThreadId())) << QThread::currentThread();

    ICMPMessage icmpMessage;

    icmpMessage.ipaddr = inet_addr(hostAddress().toStdString().c_str());
    icmpMessage.hIcmpFile = IcmpCreateFile();

    icmpMessage.ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(icmpMessage.SendData);
    icmpMessage.ReplyBuffer = (VOID*)malloc(icmpMessage.ReplySize);

    d->_requestWorker->send_request(icmpMessage, 6000);

//     icmpMessage.dwRetVal = IcmpSendEcho(
//         icmpMessage.hIcmpFile,
//         icmpMessage.ipaddr,
//         icmpMessage.SendData,
//         sizeof(icmpMessage.SendData),
//         NULL,
//         icmpMessage.ReplyBuffer,
//         icmpMessage.ReplySize
//         1000);

//     QString strMessage;
// 
//     if (icmpMessage.dwRetVal != 0) {
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

//     qDebug() << QString("====== ICMPServerMonitor: result = %1\n").arg(strMessage);
//     free(icmpMessage.ReplyBuffer);
}

void ICMPServerMonitor::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << QString("==== TCP CHECK: ERROR [%1]").arg(socketError);

    Q_D(ICMPServerMonitor);
    emit failed();
}
