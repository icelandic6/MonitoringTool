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

    protected:
        virtual void paintEvent(QPaintEvent *event) override;
    };
}
