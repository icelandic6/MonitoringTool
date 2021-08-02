#include "MonitoringToolWidget.h"
#include "ServerStatusWidget.h"

#include "ui/ControlButton.h"

#include <QPushButton>
#include <QGridLayout>
#include <QHostAddress>
#include <QResizeEvent>
#include <QPainter>
#include <QApplication>

class ui::MonitoringToolWidgetPrivate : public QObject
{
    Q_DECLARE_PUBLIC(MonitoringToolWidget)
    Q_DISABLE_COPY(MonitoringToolWidgetPrivate)
    MonitoringToolWidget *q_ptr = nullptr;

    QMap<ushort, ServerStatusWidget*> _statusWidgets;

    Qt::Orientation _orientation = Qt::Horizontal;
    ControlButton *_orientationButton = nullptr;
    ControlButton *_closeButton = nullptr;
    ControlButton *_minimizeButton = nullptr;
    QList<ControlButton*> _controlButtons;

    const int _rowsMax = 5;

    QGridLayout *_statusesLayout = nullptr;
    QGridLayout *_buttonsLayout = nullptr;
    QGridLayout *_mainLayout = nullptr;

public:
    explicit MonitoringToolWidgetPrivate(MonitoringToolWidget *q)
        : q_ptr(q)
    {
    }

    ~MonitoringToolWidgetPrivate() = default;

private:
    ControlButton* createControlButton(const QString &iconPath)
    {
        Q_Q(MonitoringToolWidget);

        auto button = new ControlButton(iconPath, q);
        _controlButtons.append(button);
        return button;
    }

    void createControlButtons()
    {
        _orientationButton = createControlButton(QString(":/MonitoringTool/orientation_icon.svg"));
        _minimizeButton = createControlButton(QString(":/MonitoringTool/minimize_icon.svg"));
        _closeButton = createControlButton(QString(":/MonitoringTool/close_icon.svg"));
    }

    void addServers(const QMap<ushort, QString> &serversInfo)
    {
        Q_Q(MonitoringToolWidget);

        for (const auto id : serversInfo.keys())
            _statusWidgets[id] = new ServerStatusWidget(serversInfo[id], q);
    }

    bool horizontal() const
    {
        return _orientation == Qt::Horizontal;
    }

    void updateStatusesLayout()
    {
        int rc = 0;

        if (_statusWidgets.size() <= _rowsMax)
        {
            for (const auto &sw : _statusWidgets.values())
            {
                _statusesLayout->addWidget(sw, horizontal() ? 0 : rc, horizontal() ? rc : 0, 2, 2);
                rc += 2;
            }
        }
        else
        {
            auto evenSize = _statusWidgets.size() % 2 == 0
                ? _statusWidgets.size() / 2
                : _statusWidgets.size() / 2 + 1;
            int r = 0;
            int c = 0;

            for (const auto &sw : _statusWidgets.values())
            {
                _statusesLayout->addWidget(sw, r, c, 2, 2);
                rc++;

                if (horizontal())
                {
                    r = (rc < evenSize) ? 0 : 2;
                    c += 2;
                    if (c >= _statusWidgets.size())
                        c = (_statusWidgets.size() % 2 == 0) ? 0 : 1;
                }
                else
                {
                    c = (rc < evenSize) ? 0 : 2;
                    r += 2;
                    if (r >= _statusWidgets.size())
                        r = (_statusWidgets.size() % 2 == 0) ? 0 : 1;
                }
            }
        }
    }

    void createLayouts()
    {
        Q_Q(MonitoringToolWidget);

        _statusesLayout = new QGridLayout();
        _statusesLayout->setAlignment(Qt::AlignCenter);
        _statusesLayout->setContentsMargins(QMargins(0, 0, 0, 0));
        _statusesLayout->setSpacing(0);
        updateStatusesLayout();

        _buttonsLayout = new QGridLayout();
        _buttonsLayout->setAlignment(Qt::AlignCenter);
        _buttonsLayout->setContentsMargins(QMargins(0, 0, 0, 0));
        _buttonsLayout->setSpacing(0);

        int i = 0;
        for (const auto &button : _controlButtons)
        {
            _buttonsLayout->addWidget(button, horizontal() ? 0 : i, horizontal() ? i : 0);
            i++;
        }

        _mainLayout = new QGridLayout();
        _mainLayout->setContentsMargins(QMargins(4, 4, 4, 4));
        _mainLayout->setSpacing(0);
        _mainLayout->addLayout(_statusesLayout, 0, 0);
        _mainLayout->addLayout(_buttonsLayout, horizontal() ? 0 : 1, horizontal() ? 1 : 0);

        q->setLayout(_mainLayout);
        q->setFixedSize(q->sizeHint());
    }

    void removeLayouts()
    {
        if (_statusesLayout)
            delete _statusesLayout;
        if (_buttonsLayout)
            delete _buttonsLayout;
        if (_mainLayout)
            delete _mainLayout;
    }

    void rotate()
    {
        Q_Q(MonitoringToolWidget);

        _orientation = (_orientation == Qt::Horizontal) ? Qt::Vertical : Qt::Horizontal;

        removeLayouts();
        createLayouts();
    }

    QSize sizeHint() const
    {
        Q_Q(const MonitoringToolWidget);

        auto spacing = q->layout()->spacing();
        auto m = q->layout()->contentsMargins();
        int w = 0, h = 0;
        int evenSize = _statusWidgets.size() + (_statusWidgets.size() % 2 == 0 ? 0 : 1);
        
        int cellsSize = _statusWidgets.size() <= _rowsMax
            ? (_statusWidgets.size() * ServerStatusWidget::cellSize())
            : (evenSize / 2) * ServerStatusWidget::cellSize();
        int cellsSpacing = spacing * (_statusWidgets.size() <= _rowsMax
                                     ? (_statusWidgets.size() - 1)
                                     : (evenSize / 2 - 1));

        int buttonsLength = _controlButtons.size() * ControlButton::buttonSize();
        int buttonsSpacing = spacing * (_controlButtons.size() - 1);

        if (horizontal())
        {
            w = cellsSize;
            w += cellsSpacing;
            w += buttonsLength;

            w += buttonsSpacing;
            h = ServerStatusWidget::cellSize() * (_statusWidgets.size() <= _rowsMax ? 1 : 2);
        }
        else
        {
            h = cellsSize;
            h += cellsSpacing;
            h += buttonsLength;

            h += buttonsSpacing;
            w = ServerStatusWidget::cellSize() * (_statusWidgets.size() <= _rowsMax ? 1 : 2);
        }

        return QSize(m.left() + w + m.right(), m.top() + h + m.bottom());
    }

    void minimize()
    {
        Q_Q(MonitoringToolWidget);

        q->setWindowState(Qt::WindowMinimized);
    }
};


ui::MonitoringToolWidget::MonitoringToolWidget(const QMap<ushort, QString> &serversInfo, QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MonitoringToolWidgetPrivate(this))
{
    Q_D(MonitoringToolWidget);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    d->addServers(serversInfo);
    d->createControlButtons();
    d->createLayouts();

    connect(d->_orientationButton, &ControlButton::clicked, d, &MonitoringToolWidgetPrivate::rotate);
    connect(d->_minimizeButton, &ControlButton::clicked, d, &MonitoringToolWidgetPrivate::minimize);
    connect(d->_closeButton, &ControlButton::clicked, this, &MonitoringToolWidget::closeApp);
}

ui::MonitoringToolWidget::~MonitoringToolWidget() = default;

void ui::MonitoringToolWidget::setServerStatus(ushort serverId, ServerStatus status)
{
    Q_D(MonitoringToolWidget);

    if (!d->_statusWidgets.contains(serverId))
        return;

    d->_statusWidgets[serverId]->setStatus(status);
}

QSize ui::MonitoringToolWidget::sizeHint() const
{
    Q_D(const MonitoringToolWidget);

    return d->sizeHint();
}

void ui::MonitoringToolWidget::paintEvent(QPaintEvent *event)
{
    Q_D(MonitoringToolWidget);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::transparent);

    QPainterPath path;
    path.addRoundedRect(rect(), 6.0, 6.0);
    painter.drawPath(path);

    QWidget::paintEvent(event);
}
