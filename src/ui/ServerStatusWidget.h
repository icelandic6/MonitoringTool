#pragma once

#include <QWidget>

#include "core/ServerStatus.h"

namespace ui
{
    class ServerStatusWidgetPrivate;
    class ServerStatusWidget : public QWidget
    {
        Q_OBJECT
    public:
        ServerStatusWidget(const QString &name, QWidget *parent = nullptr);
        virtual ~ServerStatusWidget() override;

        void setStatus(ServerStatus status);

    protected:
        virtual void paintEvent(QPaintEvent *event) override;

    private:
        QScopedPointer<ServerStatusWidgetPrivate> d_ptr;
        Q_DISABLE_COPY(ServerStatusWidget)
        Q_DECLARE_PRIVATE(ServerStatusWidget)
    };
}
