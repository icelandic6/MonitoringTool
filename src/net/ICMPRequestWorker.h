#pragma once

#include <QObject>

namespace net
{
    class ICMPRequestWorkerPrivate;
    class ICMPRequestWorker : public QObject
    {
        Q_OBJECT

    public:
        ICMPRequestWorker(const QString &addr, QObject *parent = nullptr);
        ~ICMPRequestWorker() override;

        void send_request();

    signals:
        void ready(bool success, int latency);

    private:
        QScopedPointer<ICMPRequestWorkerPrivate> d_ptr;
        Q_DISABLE_COPY(ICMPRequestWorker)
        Q_DECLARE_PRIVATE(ICMPRequestWorker)
    };
}
