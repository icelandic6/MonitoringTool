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

    int _horizontalHeight = 48;
    int _verticalWidth = 48;

    QBoxLayout *_serversLayout = nullptr;
    QBoxLayout *_buttonsLayout = nullptr;
    QBoxLayout *_mainLayout = nullptr;


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

    void createLayouts()
    {
        Q_Q(MonitoringToolWidget);

        _serversLayout = new QHBoxLayout();
        _serversLayout->setAlignment(Qt::AlignCenter);
        _serversLayout->setContentsMargins(QMargins(0, 0, 0, 0));
        _serversLayout->setSpacing(0);

        for (auto button : _statusWidgets.values())
            _serversLayout->addWidget(button);

        _buttonsLayout = new QHBoxLayout();
        _buttonsLayout->setAlignment(Qt::AlignCenter);
        _buttonsLayout->setContentsMargins(QMargins(0, 0, 0, 0));
        _buttonsLayout->setSpacing(0);

        for (auto button : _controlButtons)
            _buttonsLayout->addWidget(button);

        _mainLayout = new QHBoxLayout();
        _mainLayout->setContentsMargins(QMargins(4, 0, 4, 0));
        _mainLayout->setSpacing(0);
        _mainLayout->addLayout(_serversLayout);
        _mainLayout->addLayout(_buttonsLayout);
        q->setLayout(_mainLayout);
    }

    void changeOrientation()
    {
        Q_Q(MonitoringToolWidget);

        _orientation = (_orientation == Qt::Horizontal) ? Qt::Vertical : Qt::Horizontal;

        QBoxLayout *mainLayout = nullptr;
        QBoxLayout *serversLayout = nullptr;
        QBoxLayout *buttonsLayout = nullptr;

        if (_orientation == Qt::Horizontal)
        {
            mainLayout = new QHBoxLayout();
            mainLayout->setContentsMargins(4, 0, 4, 0);

            serversLayout = new QHBoxLayout();
            buttonsLayout = new QHBoxLayout();
        }
        else
        {
            mainLayout = new QVBoxLayout();
            mainLayout->setContentsMargins(0, 4, 0, 4);

            serversLayout = new QVBoxLayout();
            buttonsLayout = new QVBoxLayout();
        }

        serversLayout->setContentsMargins(0, 0, 0, 0);
        serversLayout->setSpacing(0);
        serversLayout->setAlignment(Qt::AlignCenter);

        for (auto widget : _statusWidgets.values())
            serversLayout->addWidget(widget);

        buttonsLayout->setContentsMargins(0, 0, 0, 0);
        buttonsLayout->setSpacing(0);
        buttonsLayout->setAlignment(Qt::AlignCenter);

        for (auto button : _controlButtons)
           buttonsLayout->addWidget(button);

        mainLayout->setSpacing(0);
        mainLayout->setAlignment(Qt::AlignCenter);
        mainLayout->addLayout(serversLayout);
        mainLayout->addLayout(buttonsLayout);

        delete _serversLayout;
        delete _buttonsLayout;
        delete _mainLayout;

        _serversLayout = serversLayout;
        _buttonsLayout = buttonsLayout;
        _mainLayout = mainLayout;

        q->setLayout(_mainLayout);
        q->setFixedSize(q->sizeHint());
    }

    void addServerStatusWidget(ushort id, const QString &name)
    {
        Q_Q(MonitoringToolWidget);

        _statusWidgets[id] = new ServerStatusWidget(name, q);
    }

    QSize sizeHint() const
    {
        Q_Q(const MonitoringToolWidget);

        QSize size;
        auto spacing = q->layout()->spacing();
        auto m = q->layout()->contentsMargins();
        int w = 0, h = 0;

        if (_orientation == Qt::Horizontal)
        {
//             qDebug() << "==== MTWIDGET:SizeHint: horizontal all = " <<
//                 QSize(m.left() +
//                     _orientationButton->width() +
//                     _minimizeButton->width() +
//                     _closeButton->width() +
//                     space * 3 +
//                     m.right(),
//                     _horizontalHeight) << "\n"
//                 << "m.left = " << m.left() << "\n"
//                 << "ori.width = " << _orientationButton->width() << "\n"
//                 << "min.width = " << _minimizeButton->width() << "\n"
//                 << "cls.width = " << _closeButton->width() << "\n"
//                 << "space = " << space << "\n"
//                 << "spacex3 = " << space * 3 << "\n"
//                 << "m.right = " << m.right() << "\n";

            for (auto sw : _statusWidgets)
                w += sw->width();

            w += spacing * (_statusWidgets.values().length() - 1);

            for (auto button : _controlButtons)
                w += button->width();

            w += spacing * (_controlButtons.length() - 1);

            return QSize(m.left() + w + m.right(), _horizontalHeight);
        }
        else
        {
//             qDebug() << "==== MTWIDGET:SizeHint: vertical all = " <<
//                 QSize(_verticalWidth, 
//                     m.top() +
//                     _orientationButton->height() +
//                     space * 3 +
//                     m.bottom()) << "\n"
//                 << "m.top = " << m.top() << "\n"
//                 << "ori.height = " << _orientationButton->height() << "\n"
//                 << "space = " << space << "\n"
//                 << "spacex3 = " << space * 3 << "\n"
//                 << "m.bottom = " << m.bottom() << "\n";
            for (auto sw : _statusWidgets)
                w += sw->height();

            w += spacing * (_statusWidgets.values().length() - 1);

            for (auto button : _controlButtons)
                w += button->height();

            w += spacing * (_controlButtons.length() - 1);

            return QSize(_verticalWidth, m.top() + w + m.bottom());
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
