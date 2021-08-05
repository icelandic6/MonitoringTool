#pragma once

#include <QWidget>

namespace ui
{
    class Separator : public QWidget
    {
    public:
        explicit Separator(QWidget *parent);
        ~Separator();

        void setOrientation(Qt::Orientation orientation);

    protected:
        void paintEvent(QPaintEvent *event) override;
    };
}
