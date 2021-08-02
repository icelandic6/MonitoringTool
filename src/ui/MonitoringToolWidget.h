#pragma once

#include <QWidget>
#include <QSystemTrayIcon>

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

        void setServerStatus(ushort serverId, core::ServerStatus status);
        void setTrayServerStatus(const QString &name, core::ServerStatus status);

        QSize sizeHint() const override;

    protected:
        virtual void paintEvent(QPaintEvent *event) override;

    signals:
        void closeApp();

    private:
        QScopedPointer<MonitoringToolWidgetPrivate> d_ptr;
        Q_DISABLE_COPY(MonitoringToolWidget)
        Q_DECLARE_PRIVATE(MonitoringToolWidget)
    };
}