#pragma once

#include <QWidget>

namespace ui
{
    class Separator : public QWidget
    {
        Q_OBJECT
    public:
        explicit Separator(QWidget *parent);
        ~Separator();

        void setOrientation(Qt::Orientation orientation);

    protected:
        virtual void paintEvent(QPaintEvent *event) override;
    };
}
