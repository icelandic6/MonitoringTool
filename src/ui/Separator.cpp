#include "Separator.h"

#include <QPainter>

ui::Separator::Separator(QWidget *parent)
    : QWidget(parent)
{
}

ui::Separator::~Separator() = default;

void ui::Separator::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal)
        setFixedSize(1, 40);
    else
        setFixedSize(40, 1);
}

void ui::Separator::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(114, 120, 129));
    QWidget::paintEvent(event);
}
