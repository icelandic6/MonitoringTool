#include "ControlButton.h"

#include <QMouseEvent>
#include <QLabel>
#include <QHBoxLayout>


ui::ControlButton::ControlButton(const QString &iconPath, QWidget *parent)
    : QWidget(parent)
{
    auto label = new QLabel(this);
    label->setPixmap(QPixmap(iconPath));

    auto layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);

    setLayout(layout);
}

ui::ControlButton::~ControlButton() = default;

void ui::ControlButton::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    emit clicked();
}
