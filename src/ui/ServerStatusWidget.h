#pragma once

#include <QWidget>

#include "core/ServerStatus.h"

namespace ui
{
    class ServerStatusWidgetPrivate;
    class ServerStatusWidget : public QWidget
    {
    public:
        ServerStatusWidget(const QString &name, QWidget *parent = nullptr);
        ~ServerStatusWidget() override;

        QString name() const;

        void setStatus(core::ServerStatus status);
        void setLatency(int latency);

        static int cellSize();

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        QScopedPointer<ServerStatusWidgetPrivate> d_ptr;
        Q_DISABLE_COPY(ServerStatusWidget)
        Q_DECLARE_PRIVATE(ServerStatusWidget)
    };
}
