#pragma once

#include <QWidget>

namespace ui
{
    class ControlButton : public QWidget
    {
        Q_OBJECT
    public:
        ControlButton(const QString &iconPath, QWidget *parent = nullptr);
        ~ControlButton();

        static int buttonSize();

    protected:
        void mouseReleaseEvent(QMouseEvent *event) override;

    signals:
        void clicked();
    };
}
