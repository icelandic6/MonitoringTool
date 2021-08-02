#include "ServerStatusWidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

static int cellSize = 40;

class ui::ServerStatusWidgetPrivate : public QObject
{
    Q_DECLARE_PUBLIC(ServerStatusWidget)
    Q_DISABLE_COPY(ServerStatusWidgetPrivate)
    ServerStatusWidget *q_ptr = nullptr;

    QString _name;

    QColor _color = QColor(100, 200, 100);
    qreal _radius = 11.5;

public:
    explicit ServerStatusWidgetPrivate(ServerStatusWidget *q)
        : q_ptr(q)
    {
    }

    ~ServerStatusWidgetPrivate() = default;
};

ui::ServerStatusWidget::ServerStatusWidget(const QString &name, QWidget *parent)
    : QWidget(parent)
    , d_ptr(new ServerStatusWidgetPrivate(this))
{
    Q_D(ServerStatusWidget);

    setFixedSize(::cellSize, ::cellSize);

    d->_name = name;

    setToolTip(name);
}

ui::ServerStatusWidget::~ServerStatusWidget() = default;

void ui::ServerStatusWidget::setStatus(core::ServerStatus status)
{
    Q_D(ServerStatusWidget);

    if (status == core::ServerStatus::Available)
        d->_color = QColor(100, 200, 100);
    else if (status == core::ServerStatus::Unstable)
        d->_color = QColor(200, 200, 100);
    else if (status == core::ServerStatus::Failed)
        d->_color = QColor(200, 100, 100);

    update();
}

void ui::ServerStatusWidget::paintEvent(QPaintEvent *event)
{
    Q_D(ServerStatusWidget);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(d->_color);
    painter.setPen(d->_color);

    painter.fillRect(rect(), QColor(100, 100, 200, 100));
    painter.drawEllipse(QPointF(rect().width() / 2, rect().height() / 2), d->_radius, d->_radius);

    QWidget::paintEvent(event);
}

int ui::ServerStatusWidget::cellSize()
{
    return ::cellSize;
}
