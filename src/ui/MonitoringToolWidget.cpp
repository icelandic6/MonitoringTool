#include "MonitoringToolWidget.h"
#include "ServerStatusWidget.h"
#include "ControlButton.h"
#include "Separator.h"
#include "StatusColor.h"
#include "core/Logger.h"

#include <QGridLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include <QMenu>

class ui::MonitoringToolWidgetPrivate : public QObject
{
    Q_DECLARE_PUBLIC(MonitoringToolWidget)
    Q_DISABLE_COPY(MonitoringToolWidgetPrivate)
    MonitoringToolWidget *q_ptr = nullptr;

    QMap<ushort, ServerStatusWidget*> _statusWidgets;

    Separator *_separator = nullptr;

    Qt::Orientation _orientation = Qt::Horizontal;
    ControlButton *_orientationButton = nullptr;
    ControlButton *_closeButton = nullptr;
    ControlButton *_minimizeButton = nullptr;
    QList<ControlButton*> _controlButtons;

    QGridLayout *_statusesLayout = nullptr;
    QGridLayout *_buttonsLayout = nullptr;
    QGridLayout *_mainLayout = nullptr;

    QSystemTrayIcon *_trayIcon = nullptr;
    ushort _trayIconServerId = 0;

    QWidget *_logWindow = nullptr;
    QTextEdit *_logTextEdit = nullptr;

    QPoint _movePosition;

    QColor _backgroundColor;

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

    void createSeparator()
    {
        Q_Q(MonitoringToolWidget);

        _separator = new Separator(q);
        _separator->setOrientation(_orientation);
    }

    void createSystemTrayIcon()
    {
        Q_Q(MonitoringToolWidget);

        _trayIcon = new QSystemTrayIcon(q);

        createLogWidget();

        auto menu = new QMenu(q);
        auto showLogAction = new QAction(QString("Show Logs"), this);
        connect(showLogAction, &QAction::triggered, this, [this]()
        {
            auto logs = core::Logger::instance()->logs();
            if (logs.isEmpty())
                _logTextEdit->setText(QString("No logs here yet..."));
            while (!logs.isEmpty())
                _logTextEdit->setText(logs.takeFirst());

            _logWindow->show();
        });
        menu->addAction(showLogAction);

        _trayIcon->setContextMenu(menu);
    }

    void createLogWidget()
    {
        _logWindow = new QWidget();
        _logWindow->setWindowFlags(Qt::Window);
        _logWindow->resize(640, 480);
        _logWindow->setAttribute(Qt::WA_QuitOnClose, false);

        _logTextEdit = new QTextEdit(_logWindow);
        _logTextEdit->setReadOnly(true);
        _logTextEdit->setText(QString("No logs here yet..."));

        auto logger = core::Logger::instance();
        connect(logger, &core::Logger::logAdded, this, [this](const QString &message)
        {
            _logTextEdit->append(message);
        });

        auto logLayout = new QHBoxLayout();
        logLayout->setContentsMargins(0, 0, 0, 0);
        logLayout->setSpacing(0);
        logLayout->addWidget(_logTextEdit);

        _logWindow->setLayout(logLayout);
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

    void createLayouts()
    {
        Q_Q(MonitoringToolWidget);

        _statusesLayout = new QGridLayout();
        _statusesLayout->setAlignment(Qt::AlignCenter);
        _statusesLayout->setContentsMargins(QMargins(0, 0, 0, 0));
        _statusesLayout->setSpacing(0);

        int rc = 0;
        for (const auto &sw : _statusWidgets.values())
        {
            _statusesLayout->addWidget(sw, horizontal() ? 0 : rc, horizontal() ? rc : 0, 1, 1);
            rc++;
        }

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
        _mainLayout->setAlignment(Qt::AlignCenter);
        _mainLayout->setSpacing(0);
        _mainLayout->addLayout(_statusesLayout, 0, 0);
        _mainLayout->addWidget(_separator, horizontal() ? 0 : 1, horizontal() ? 1 : 0, Qt::AlignCenter);
        _mainLayout->addLayout(_buttonsLayout, horizontal() ? 0 : 2, horizontal() ? 2 : 0);

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

public:
    void rotate()
    {
        Q_Q(MonitoringToolWidget);

        _orientation = (_orientation == Qt::Horizontal) ? Qt::Vertical : Qt::Horizontal;

        _separator->setOrientation(_orientation);

        removeLayouts();
        createLayouts();
    }

    QSize sizeHint() const
    {
        Q_Q(const MonitoringToolWidget);

        auto spacing = q->layout()->spacing();
        auto m = q->layout()->contentsMargins();
        
        int cellsSize = _statusWidgets.size() * ServerStatusWidget::cellSize();
        int cellsSpacing = spacing * (_statusWidgets.size() - 1);

        int buttonsLength = _controlButtons.size() * ControlButton::buttonSize();
        int buttonsSpacing = spacing * (_controlButtons.size() - 1);
        int separatorLength = spacing * 2 + (horizontal() ? _separator->width() : _separator->height());

        int wholeLength = cellsSize + cellsSpacing + separatorLength + buttonsLength + buttonsSpacing;

        int w = horizontal() ? wholeLength : ServerStatusWidget::cellSize();
        int h = horizontal() ? ServerStatusWidget::cellSize() : wholeLength;

        return QSize(m.left() + w + m.right(), m.top() + h + m.bottom());
    }

    QRect movingRect() const
    {
        Q_Q(const MonitoringToolWidget);

        auto m = q->layout()->contentsMargins();
        int cellsSize = _statusWidgets.size() * ServerStatusWidget::cellSize();
        int cellsSpacing = q->layout()->spacing() * _statusWidgets.size();

        int w = horizontal() ? (cellsSize + cellsSpacing) : ServerStatusWidget::cellSize();
        int h = horizontal() ? ServerStatusWidget::cellSize() : cellsSize + cellsSpacing;

        return QRect(0, 0, m.left() + w + m.right(), m.top() + h + m.bottom());
    }

    void minimize()
    {
        Q_Q(MonitoringToolWidget);

        q->setWindowState(Qt::WindowMinimized);
        q->hide();
    }

    void trayIconActivated(QSystemTrayIcon::ActivationReason reason)
    {
        Q_Q(MonitoringToolWidget);

        if (reason == QSystemTrayIcon::ActivationReason::Context)
            _trayIcon->contextMenu()->show();
        else
        {
            q->show();
            q->setWindowState(Qt::WindowActive);
        }
    }

    void setTrayServerStatus(core::ServerStatus status)
    {
        Q_Q(MonitoringToolWidget);

        QColor color = statusColor(status);

        QPixmap trayPixmap(16, 16);
        trayPixmap.fill(Qt::transparent);

        QPainter painter(&trayPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(color);
        painter.setPen(color);
        painter.drawEllipse(QPointF(trayPixmap.width() / 2, trayPixmap.height() / 2), 7, 7);

        _trayIcon->setIcon(QIcon(trayPixmap));

        if (!_trayIcon->isVisible())
            _trayIcon->show();
    }
};


ui::MonitoringToolWidget::MonitoringToolWidget(const QMap<ushort, QString> &serversInfo, QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MonitoringToolWidgetPrivate(this))
{
    Q_D(MonitoringToolWidget);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SubWindow);
    setAttribute(Qt::WA_TranslucentBackground);

    d->_backgroundColor = core::AppSettings::instance()->backgroundColor();

    d->addServers(serversInfo);
    d->createSeparator();
    d->createControlButtons();
    d->createLayouts();
    d->createSystemTrayIcon();

    connect(d->_orientationButton, &ControlButton::clicked, d, &MonitoringToolWidgetPrivate::rotate);
    connect(d->_minimizeButton, &ControlButton::clicked, d, &MonitoringToolWidgetPrivate::minimize);
    connect(d->_closeButton, &ControlButton::clicked, this, &MonitoringToolWidget::closeApp);
    connect(d->_trayIcon, &QSystemTrayIcon::activated, d, &MonitoringToolWidgetPrivate::trayIconActivated);
}

ui::MonitoringToolWidget::~MonitoringToolWidget()
{
    Q_D(MonitoringToolWidget);

    if (d->_logWindow)
        delete d->_logWindow;
}

void ui::MonitoringToolWidget::setServerLatency(ushort serverId, int latency)
{
    Q_D(MonitoringToolWidget);

    if (!d->_statusWidgets.contains(serverId))
        return;

    d->_statusWidgets[serverId]->setLatency(latency);
}

void ui::MonitoringToolWidget::setServerStatus(ushort serverId, core::ServerStatus status)
{
    Q_D(MonitoringToolWidget);

    if (!d->_statusWidgets.contains(serverId))
        return;

    d->_statusWidgets[serverId]->setStatus(status);
}

void ui::MonitoringToolWidget::setTrayServerStatus(ushort serverId, core::ServerStatus status)
{
    Q_D(MonitoringToolWidget);

    if (!d->_statusWidgets.contains(serverId))
        return;

    d->_trayIconServerId = serverId;

    d->setTrayServerStatus(status);
}

void ui::MonitoringToolWidget::setTrayServerTooltip(ushort serverId, int latency)
{
    Q_D(MonitoringToolWidget);

    if (!d->_statusWidgets.contains(serverId) || d->_trayIconServerId != serverId)
        return;

    QString tooltip = d->_statusWidgets[serverId]->name();
    if (latency)
        tooltip += QString(" %1ms").arg(latency);

    if (d->_trayIcon->toolTip() != tooltip)
        d->_trayIcon->setToolTip(tooltip);
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
    painter.setBrush(d->_backgroundColor);
    painter.setPen(Qt::transparent);

    QPainterPath path;
    path.addRoundedRect(rect(), 6.0, 6.0);
    painter.drawPath(path);

    QWidget::paintEvent(event);
}

void ui::MonitoringToolWidget::mousePressEvent(QMouseEvent *event)
{
    Q_D(MonitoringToolWidget);

    if (d->movingRect().contains(event->pos()))
        d->_movePosition = event->pos();

    QWidget::mousePressEvent(event);
}

void ui::MonitoringToolWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(MonitoringToolWidget);

    if (!d->_movePosition.isNull())
        this->move(pos() + event->pos() - d->_movePosition);

    QWidget::mouseMoveEvent(event);
}

void ui::MonitoringToolWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(MonitoringToolWidget);

    d->_movePosition = QPoint();

    QWidget::mouseMoveEvent(event);
}
