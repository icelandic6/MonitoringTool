#pragma once

#include <QObject>

#include "ICMPMessage.h"

class ICMPRequestWorkerPrivate;
class ICMPRequestWorker : public QObject
{
    Q_OBJECT

public:
    explicit ICMPRequestWorker(QObject *parent = nullptr);
    virtual ~ICMPRequestWorker();

    void send_request(ICMPMessage &message, int timeout = 1000);

signals:
    void ready(bool success);

//     void success();
//     void fail();

private:
    QScopedPointer<ICMPRequestWorkerPrivate> d_ptr;
    Q_DISABLE_COPY(ICMPRequestWorker)
    Q_DECLARE_PRIVATE(ICMPRequestWorker)
};
