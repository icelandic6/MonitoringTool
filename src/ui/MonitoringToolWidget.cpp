#include "MonitoringToolWidget.h"
#include "ServerStatusWidget.h"

#include "ui/ControlButton.h"

#include <QPushButton>
#include <QGridLayout>
#include <QHostAddress>
#include <QResizeEvent>
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

    const int _mainMargin = 4;
    const int _cellsLength = 48;

    const int _rowsMax = 5;

    QGridLayout *_statusesLayout = nullptr;
    QGridLayout *_buttonsLayout = nullptr;
    QGridLayout *_mainLayout = nullptr;

public:
    explicit MonitoringToolWidgetPrivate(MonitoringToolWidget *q)
        : q_ptr(q)
    {
    }

    ~MonitoringToolWidgetPrivate()
    {
    }

private:
    ControlButton* createControlButton(const QString &iconPath)
    {
        Q_Q(MonitoringToolWidget);

        auto button = new ControlButton(iconPath, q);
        button->setFixedSize(32, 32);

        _controlButtons.append(button);
        return button;
    }

    void createControlButtons()
    {
        _orientationButton = createControlButton(QString(":/MonitoringTool/orientation_icon.svg"));
        _minimizeButton = createControlButton(QString(":/MonitoringTool/minimize_icon.svg"));
        _closeButton = createControlButton(QString(":/MonitoringTool/close_icon.svg"));
    }

    void createServers(const QMap<ushort, QString> &serversInfo)
    {
        Q_Q(MonitoringToolWidget);

        for (auto id : serversInfo.keys())
            _statusWidgets[id] = new ServerStatusWidget(serversInfo[id], q);
    }

    bool horizontal() const
    {
        return _orientation == Qt::Horizontal;
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

    void createLayouts()
    {
        Q_Q(MonitoringToolWidget);

        _statusesLayout = new QGridLayout();
        _statusesLayout->setAlignment(Qt::AlignCenter);
        _statusesLayout->setContentsMargins(QMargins(0, 0, 0, 0));
        _statusesLayout->setSpacing(0);

        if (_statusWidgets.size() <= _rowsMax)
        {
            int rc = 0;
            for (auto sw : _statusWidgets.values())
            {
                _statusesLayout->addWidget(sw, horizontal() ? 0 : rc, horizontal() ? rc : 0, 2, 2);
                rc += 2;
            }
        }
        else
        {
            int rc = 0;
            auto evenSize = _statusWidgets.size() % 2 == 0
                ? _statusWidgets.size() / 2
                : _statusWidgets.size() / 2 + 1;
            int r = 0;
            int c = 0;

            for (auto sw : _statusWidgets.values())
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

        _buttonsLayout = new QGridLayout();
        _buttonsLayout->setAlignment(Qt::AlignCenter);
        _buttonsLayout->setContentsMargins(QMargins(0, 0, 0, 0));
        _buttonsLayout->setSpacing(0);

        int i = 0;
        for (auto button : _controlButtons)
        {
            _buttonsLayout->addWidget(button, horizontal() ? 0 : i, horizontal() ? i : 0);
            i++;
        }

        _mainLayout = new QGridLayout();
        _mainLayout->setContentsMargins(QMargins(
            horizontal() ? _mainMargin : 0,
            horizontal() ? 0 : _mainMargin,
            horizontal() ? _mainMargin : 0,
            horizontal() ? 0 : _mainMargin
        ));
        _mainLayout->setSpacing(0);
        _mainLayout->addLayout(_statusesLayout, 0, 0);
        _mainLayout->addLayout(_buttonsLayout, horizontal() ? 0 : 1, horizontal() ? 1 : 0);

        q->setLayout(_mainLayout);
        q->setFixedSize(q->sizeHint());
    }

    void changeOrientation()
    {
        Q_Q(MonitoringToolWidget);

        _orientation = (_orientation == Qt::Horizontal) ? Qt::Vertical : Qt::Horizontal;

        removeLayouts();
        createLayouts();
    }

    void addServerStatusWidget(ushort id, const QString &name)
    {
        Q_Q(MonitoringToolWidget);

        _statusWidgets[id] = new ServerStatusWidget(name, q);
    }

    QSize sizeHint() const
    {
        Q_Q(const MonitoringToolWidget);

        auto spacing = q->layout()->spacing();
        auto m = q->layout()->contentsMargins();
        int w = 0, h = 0;
        int evenSize = _statusWidgets.size() + (_statusWidgets.size() % 2 == 0 ? 0 : 1);

        if (horizontal())
        {
            w = _statusWidgets.size() <= _rowsMax
                ? (_statusWidgets.size() * ServerStatusWidget::sizeSize())
                : (evenSize / 2) * ServerStatusWidget::sizeSize();

            w += spacing * (_statusWidgets.size() <= _rowsMax
                           ? (_statusWidgets.size() - 1)
                           : (evenSize / 2 - 1));

            for (auto button : _controlButtons)
                w += button->width();

            w += spacing * (_controlButtons.length() - 1);
            h = _cellsLength * (_statusWidgets.size() <= _rowsMax ? 1 : 2);

            return QSize(m.left() + w + m.right(), h);
        }
        else
        {
            h = _statusWidgets.size() <= _rowsMax
                ? (_statusWidgets.size() * ServerStatusWidget::sizeSize())
                : (evenSize / 2) * ServerStatusWidget::sizeSize();

            h += spacing * (_statusWidgets.size() <= _rowsMax
                ? (_statusWidgets.size() - 1)
                : (evenSize / 2 - 1));

            for (auto button : _controlButtons)
                h += button->height();

            h += spacing * (_controlButtons.length() - 1);
            w = _cellsLength * (_statusWidgets.size() <= _rowsMax ? 1 : 2);

            return QSize(w, m.top() + h + m.bottom());
        }
    }

    void minimizeApp()
    {
        Q_Q(MonitoringToolWidget);

        q->setWindowState(Qt::WindowMinimized);
    }

    void closeApp()
    {
        qApp->exit();
    }
};


ui::MonitoringToolWidget::MonitoringToolWidget(const QMap<ushort, QString> &serversInfo, QWidget *parent)
    : QWidget(parent)
	, d_ptr(new MonitoringToolWidgetPrivate(this))
{
	Q_D(MonitoringToolWidget);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    d->createServers(serversInfo);
    d->createControlButtons();
	d->createLayouts();

    connect(d->_orientationButton, &ControlButton::clicked, d, &MonitoringToolWidgetPrivate::changeOrientation);
    connect(d->_minimizeButton, &ControlButton::clicked, d, &MonitoringToolWidgetPrivate::minimizeApp);
    connect(d->_closeButton, &ControlButton::clicked, d, &MonitoringToolWidgetPrivate::closeApp);

}

ui::MonitoringToolWidget::~MonitoringToolWidget()
{
    //TODO: check if destructor called on app quit
    qDebug() << "\n==== MonitoringToolWidget: DESTROYED\n";
}

void ui::MonitoringToolWidget::setServerStatus(ushort serverId, ServerStatus status)
{
    Q_D(MonitoringToolWidget);

    if (!d->_statusWidgets.contains(serverId))
        return;

    d->_statusWidgets[serverId]->setStatus(status);
}

// void ui::MonitoringToolWidget::addServerStatusWidget(ushort id, const QString &name)
// {
//     Q_D(MonitoringToolWidget);
// 
//     d->addServerStatusWidget(id, name);
// }

QSize ui::MonitoringToolWidget::sizeHint() const
{
    Q_D(const MonitoringToolWidget);

    return d->sizeHint();
}

void ui::MonitoringToolWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() << "===== MTWIDGET:RESIZE_EVENT: old size = [" << event->oldSize() << "], new size = [" << event->size() << "]\n";
}
