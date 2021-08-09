#pragma once

#include <QObject>

namespace net
{
    class UDPWorkerPrivate;
    class UDPWorker : public QObject
    {
        Q_OBJECT

    public:
        explicit UDPWorker(int port, QObject *parent = nullptr);
        ~UDPWorker() override;

        void start(const QString &addr);

    signals:
        void ready(bool success);

    private:
        QScopedPointer<UDPWorkerPrivate> d_ptr;
        Q_DISABLE_COPY(UDPWorker)
        Q_DECLARE_PRIVATE(UDPWorker)
    };
}
