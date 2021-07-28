#pragma once

#include <QWidget>

namespace ui
{
    class MonitoringToolWidgetPrivate;
    class MonitoringToolWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit MonitoringToolWidget(QWidget *parent = nullptr);
        ~MonitoringToolWidget();

        QSize sizeHint() const override;

        virtual void resizeEvent(QResizeEvent *event) override;

    private:
        QScopedPointer<MonitoringToolWidgetPrivate> d_ptr;
        Q_DISABLE_COPY(MonitoringToolWidget)
        Q_DECLARE_PRIVATE(MonitoringToolWidget)
    };
}