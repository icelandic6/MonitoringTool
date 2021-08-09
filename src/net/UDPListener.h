#pragma once

#include <QObject>

namespace net
{
    class UDPListenerPrivate;
    class UDPListener : public QObject
    {
        Q_OBJECT

    public:
        explicit UDPListener(int port, QObject *parent = nullptr);
        ~UDPListener() override;

        void start();

    signals:
        void ready(bool success);

    private:
        QScopedPointer<UDPListenerPrivate> d_ptr;
        Q_DISABLE_COPY(UDPListener)
        Q_DECLARE_PRIVATE(UDPListener)
    };
}
