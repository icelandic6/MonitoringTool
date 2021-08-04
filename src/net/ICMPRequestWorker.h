#pragma once

#include <QObject>

#include "ICMPMessage.h"

class ICMPRequestWorkerPrivate;
class ICMPRequestWorker : public QObject
{
    Q_OBJECT

public:
    ICMPRequestWorker(ICMPMessage message, int timeout = 1000, QObject *parent = nullptr);
    virtual ~ICMPRequestWorker();

    void send_request();

signals:
    void ready(bool success, int latency);

private:
    QScopedPointer<ICMPRequestWorkerPrivate> d_ptr;
    Q_DISABLE_COPY(ICMPRequestWorker)
    Q_DECLARE_PRIVATE(ICMPRequestWorker)
};
