#include "ServerStatusWidget.h"
#include "StatusColor.h"
#include "core/AppSettings.h"

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

    QColor _color;
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

    setStatus(core::ServerStatus::Available);
    setToolTip(name);
}

ui::ServerStatusWidget::~ServerStatusWidget() = default;

QString ui::ServerStatusWidget::name() const
{
    Q_D(const ServerStatusWidget);

    return d->_name;
}

void ui::ServerStatusWidget::setStatus(core::ServerStatus status)
{
    Q_D(ServerStatusWidget);

    d->_color = ui::statusColor(status);
    update();
}

void ui::ServerStatusWidget::setLatency(int latency)
{
    Q_D(ServerStatusWidget);

    auto tooltip = QString("%1 %2ms").arg(d->_name).arg(latency);
    if (tooltip != this->toolTip())
        setToolTip(tooltip);
}

void ui::ServerStatusWidget::paintEvent(QPaintEvent *event)
{
    Q_D(ServerStatusWidget);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(d->_color);
    painter.setPen(d->_color);
    painter.drawEllipse(QPointF(rect().width() / 2, rect().height() / 2), d->_radius, d->_radius);

    QWidget::paintEvent(event);
}

int ui::ServerStatusWidget::cellSize()
{
    return ::cellSize;
}
