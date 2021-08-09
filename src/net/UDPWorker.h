#pragma once

#include <QObject>

namespace net
{
    class UDPWorkerPrivate;
    class UDPWorker : public QObject
    {
        Q_OBJECT

    public:
        explicit UDPWorker(const QString &address, int port, QObject *parent = nullptr);
        ~UDPWorker() override;

        void start();

    signals:
        void ready(bool success);

    private:
        QScopedPointer<UDPWorkerPrivate> d_ptr;
        Q_DISABLE_COPY(UDPWorker)
        Q_DECLARE_PRIVATE(UDPWorker)
    };
}
