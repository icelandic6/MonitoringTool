#include "ControlButton.h"

#include <QMouseEvent>
#include <QLabel>
#include <QHBoxLayout>

static int buttonSize = 32;

ui::ControlButton::ControlButton(const QString &iconPath, QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(::buttonSize, ::buttonSize);

    auto label = new QLabel(this);
    label->setPixmap(QPixmap(iconPath));

    auto layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);

    setLayout(layout);
}

ui::ControlButton::~ControlButton() = default;

int ui::ControlButton::buttonSize()
{
    return ::buttonSize;
}

void ui::ControlButton::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked();

    QWidget::mouseReleaseEvent(event);
}
