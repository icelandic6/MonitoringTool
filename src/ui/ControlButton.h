#pragma once

#include <QWidget>

namespace ui
{
    class ControlButton : public QWidget
    {
        Q_OBJECT
    public:
        explicit ControlButton(const QString &iconPath, QWidget *parent = nullptr);
        ~ControlButton();

    protected:
        virtual void mouseReleaseEvent(QMouseEvent *event) override;
        virtual void paintEvent(QPaintEvent *event) override;

    signals:
        void clicked();
    };
}