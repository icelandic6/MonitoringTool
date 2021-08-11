#pragma once

#include <QObject>

namespace net
{
    class ICMPWorkerPrivate;
    class ICMPWorker : public QObject
    {
        Q_OBJECT

    public:
        ICMPWorker(const QString &addr, QObject *parent = nullptr);
        ~ICMPWorker() override;

        void send_request();

    signals:
        void ready(bool success, int latency);

    private:
        QScopedPointer<ICMPWorkerPrivate> d_ptr;
        Q_DISABLE_COPY(ICMPWorker)
        Q_DECLARE_PRIVATE(ICMPWorker)
    };
}
