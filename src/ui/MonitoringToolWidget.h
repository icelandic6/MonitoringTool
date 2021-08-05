#pragma once

#include <QWidget>

#include "core/ServerStatus.h"

namespace ui
{
    class MonitoringToolWidgetPrivate;
    class MonitoringToolWidget : public QWidget
    {
        Q_OBJECT

    public:
        MonitoringToolWidget(const QMap<ushort, QString> &serversInfo, QWidget *parent = nullptr);
        ~MonitoringToolWidget();

        void setServerLatency(ushort serverId, int latency);
        void setServerStatus(ushort serverId, core::ServerStatus status);

        void setTrayServerStatus(ushort serverId, core::ServerStatus status);
        void setTrayServerTooltip(ushort serverId, int latency = 0);

        QSize sizeHint() const override;

    protected:
        void paintEvent(QPaintEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;

    signals:
        void closeApp();

    private:
        QScopedPointer<MonitoringToolWidgetPrivate> d_ptr;
        Q_DISABLE_COPY(MonitoringToolWidget)
        Q_DECLARE_PRIVATE(MonitoringToolWidget)
    };
}