#include "MonitoringToolWidget.h"
#include "net/TCPServerMonitor.h"
#include "net/UDPServerMonitor.h"
#include "net/ICMPServerMonitor.h"

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

    QPushButton *_checkTCPServerButton = nullptr;
    QPushButton *_checkUDPServerButton = nullptr;
    QPushButton *_checkICMPServerButton = nullptr;

    Qt::Orientation _orientation = Qt::Vertical;
    ControlButton *_orientationButton = nullptr;
    ControlButton *_closeButton = nullptr;
    ControlButton *_minimizeButton = nullptr;

    int _horizontalHeight = 48;
    int _verticalWidth = 48;

public:
    explicit MonitoringToolWidgetPrivate(MonitoringToolWidget *q)
        : q_ptr(q)
    {
    }

    ~MonitoringToolWidgetPrivate()
    {
    }

private:
    QPushButton* createServerButton(const QString &name, const QString &iconPath = QString())
    {
        Q_Q(MonitoringToolWidget);

        auto button = new QPushButton(name, q);
        button->setFixedSize(48, 48);
        button->setAutoFillBackground(true);

        if (!iconPath.isEmpty())
            button->setIcon(QIcon(iconPath));

        connect(button, &QPushButton::pressed, this, [button]()
        {
            QPalette pal = button->palette();
            pal.setColor(QPalette::Button, QColor(100, 100, 100));
            button->setPalette(pal);
        });

        return button;
    }

    void createCheckServerButtons()
    {
        Q_Q(MonitoringToolWidget);

        _checkTCPServerButton = createServerButton("TCP");
        _checkUDPServerButton = createServerButton("UDP");
        _checkICMPServerButton = createServerButton("ICMP");
    }

    void createOrientationButton()
    {
        Q_Q(MonitoringToolWidget);

        _orientationButton = new ControlButton(":/MonitoringTool/orientation_icon.svg", q);
        _orientationButton->setFixedSize(32, 32);
    }

    void createMinimizeButton()
    {
        Q_Q(MonitoringToolWidget);
        _minimizeButton = new ControlButton(":/MonitoringTool/minimize_icon.svg", q);
        _minimizeButton->setFixedSize(32, 32);
    }

    void createCloseButton()
    {
        Q_Q(MonitoringToolWidget);
        _closeButton = new ControlButton(":/MonitoringTool/close_icon.svg", q);
        _closeButton->setFixedSize(32, 32);
    }

    void createLayouts()
    {
        Q_Q(MonitoringToolWidget);

        auto mainLayout = new QVBoxLayout();
        mainLayout->setContentsMargins(QMargins(0, 0, 0, 0));
        mainLayout->setSpacing(0);
        mainLayout->addWidget(_checkTCPServerButton, 0, Qt::AlignCenter);
        mainLayout->addWidget(_checkUDPServerButton, 0, Qt::AlignCenter);
        mainLayout->addWidget(_checkICMPServerButton, 0, Qt::AlignCenter);

        mainLayout->addWidget(_orientationButton, 0, Qt::AlignCenter);
        mainLayout->addWidget(_minimizeButton, 0, Qt::AlignCenter);
        mainLayout->addWidget(_closeButton , 0, Qt::AlignCenter);

        q->setLayout(mainLayout);
    }

    void changeOrientation()
    {
        Q_Q(MonitoringToolWidget);

        _orientation = (_orientation == Qt::Horizontal) ? Qt::Vertical : Qt::Horizontal;

        auto spacing = q->layout()->spacing();
        auto margins = q->layout()->contentsMargins();

        QLayout *layout;

        if (_orientation == Qt::Horizontal)
            layout = new QHBoxLayout();
        else
            layout = new QVBoxLayout();

        layout->setContentsMargins(margins);
        layout->setSpacing(spacing);

        layout->addWidget(_checkTCPServerButton);
        layout->addWidget(_checkUDPServerButton);
        layout->addWidget(_checkICMPServerButton);
        layout->addWidget(_orientationButton);
        layout->addWidget(_minimizeButton);
        layout->addWidget(_closeButton);

        delete q->layout();

        q->setLayout(layout);
        q->setFixedSize(q->sizeHint());
    }

    QSize sizeHint() const
    {
        Q_Q(const MonitoringToolWidget);

        QSize size;
        auto space = q->layout()->spacing();
        auto m = q->layout()->contentsMargins();
        int w, h = 0;

        if (_orientation == Qt::Horizontal)
        {
//             qDebug() << "==== MTWIDGET:SizeHint: horizontal all = " <<
//                 QSize(m.left() +
//                     _checkTCPServerButton->width() +
//                     _checkTCPServerButton->width() +
//                     _checkTCPServerButton->width() +
//                     _orientationButton->width() +
//                     _minimizeButton->width() +
//                     _closeButton->width() +
//                     space * 3 +
//                     m.right(),
//                     _horizontalHeight) << "\n"
//                 << "m.left = " << m.left() << "\n"
//                 << "tcp.width = " <<_checkTCPServerButton->width() << "\n"
//                 << "udp.width = " << _checkTCPServerButton->width() << "\n"
//                 << "icmp.width = " << _checkTCPServerButton->width() << "\n"
//                 << "ori.width = " << _orientationButton->width() << "\n"
//                 << "min.width = " << _minimizeButton->width() << "\n"
//                 << "cls.width = " << _closeButton->width() << "\n"
//                 << "space = " << space << "\n"
//                 << "spacex3 = " << space * 3 << "\n"
//                 << "m.right = " << m.right() << "\n";

            return QSize(m.left() +
                         _checkTCPServerButton->width() +
                         _checkTCPServerButton->width() +
                         _checkTCPServerButton->width() +
                         _orientationButton->width() +
                         _minimizeButton->width() +
                         _closeButton->width() +
                         space * 5 +
                         m.right(),
                         _horizontalHeight);
        }
        else
        {
//             qDebug() << "==== MTWIDGET:SizeHint: vertical all = " <<
//                 QSize(_verticalWidth, 
//                     m.top() +
//                     _checkTCPServerButton->height() +
//                     _checkTCPServerButton->height() +
//                     _checkTCPServerButton->height() +
//                     _orientationButton->height() +
//                     space * 3 +
//                     m.bottom()) << "\n"
//                 << "m.top = " << m.top() << "\n"
//                 << "tcp.height = " << _checkTCPServerButton->height() << "\n"
//                 << "udp.height = " << _checkTCPServerButton->height() << "\n"
//                 << "icmp.height = " << _checkTCPServerButton->height() << "\n"
//                 << "ori.height = " << _orientationButton->height() << "\n"
//                 << "space = " << space << "\n"
//                 << "spacex3 = " << space * 3 << "\n"
//                 << "m.bottom = " << m.bottom() << "\n";

            return QSize(_verticalWidth,
                m.top() +
                _checkTCPServerButton->height() +
                _checkTCPServerButton->height() +
                _checkTCPServerButton->height() +
                _orientationButton->height() +
                _minimizeButton->height() +
                _closeButton->height() +
                space * 5 +
                m.bottom());
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


ui::MonitoringToolWidget::MonitoringToolWidget(QWidget *parent)
    : QWidget(parent)
	, d_ptr(new MonitoringToolWidgetPrivate(this))
{
	Q_D(MonitoringToolWidget);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    d->createOrientationButton();
    d->createMinimizeButton();
    d->createCloseButton();
	d->createCheckServerButtons();
	d->createLayouts();

    connect(d->_orientationButton, &ControlButton::clicked, d, &MonitoringToolWidgetPrivate::changeOrientation);
    connect(d->_minimizeButton, &ControlButton::clicked, d, &MonitoringToolWidgetPrivate::minimizeApp);
    connect(d->_closeButton, &ControlButton::clicked, d, &MonitoringToolWidgetPrivate::closeApp);


    connect(d->_checkTCPServerButton, &QPushButton::clicked, this, &MonitoringToolWidget::checkTCPServer);
    connect(d->_checkUDPServerButton, &QPushButton::clicked, this, &MonitoringToolWidget::checkUDPServer);
    connect(d->_checkICMPServerButton, &QPushButton::clicked, this, &MonitoringToolWidget::checkICMPServer);
}

ui::MonitoringToolWidget::~MonitoringToolWidget()
{
    qDebug() << "\n==== MonitoringToolWidget: DESTROYED\n";
}

QSize ui::MonitoringToolWidget::sizeHint() const
{
    Q_D(const MonitoringToolWidget);

    return d->sizeHint();
}

void ui::MonitoringToolWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() << "===== MTWIDGET:RESIZE_EVENT: old size = [" << event->oldSize() << "], new size = [" << event->size() << "]\n";
}
