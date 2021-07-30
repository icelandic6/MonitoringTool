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
        explicit MonitoringToolWidget(const QMap<ushort, QString> &serversInfo, QWidget *parent = nullptr);
        ~MonitoringToolWidget();

        void setServerStatus(ushort serverId, ServerStatus status);

        QSize sizeHint() const override;

    protected:
        virtual void resizeEvent(QResizeEvent *event) override;

    signals:
        void checkTCPServer();
        void checkUDPServer();
        void checkICMPServer();

    private:
        QScopedPointer<MonitoringToolWidgetPrivate> d_ptr;
        Q_DISABLE_COPY(MonitoringToolWidget)
        Q_DECLARE_PRIVATE(MonitoringToolWidget)
    };
}