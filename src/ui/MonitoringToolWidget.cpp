#include "MonitoringToolWidget.h"
#include "net/TCPServerMonitor.h"
#include "net/UDPServerMonitor.h"
#include "net/ICMPServerMonitor.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QHostAddress>

class ui::MonitoringToolWidgetPrivate : public QObject
{
    Q_DECLARE_PUBLIC(MonitoringToolWidget)
    Q_DISABLE_COPY(MonitoringToolWidgetPrivate)
    MonitoringToolWidget *q_ptr = nullptr;

    QPushButton *_checkTCPServerButton = nullptr;
    QPushButton *_checkUDPServerButton = nullptr;
    QPushButton *_checkICMPServerButton = nullptr;

    ServerMonitor *_tcpServerMonitor = nullptr;
    ServerMonitor *_udpServerMonitor = nullptr;
    ServerMonitor *_icmpServerMonitor = nullptr;

public:
    explicit MonitoringToolWidgetPrivate(MonitoringToolWidget *q)
        : q_ptr(q)
    {
    }

    ~MonitoringToolWidgetPrivate()
    {
    }

private:
    void createCheckServerButtons()
    {
        Q_Q(MonitoringToolWidget);

        _checkTCPServerButton = new QPushButton("Check TCP server", q);
        _checkTCPServerButton->setFixedSize(100, 30);
        _checkTCPServerButton->setAutoFillBackground(true);

        connect(_checkTCPServerButton, &QPushButton::pressed, this, [this]()
        {
            QPalette pal = _checkTCPServerButton->palette();
            pal.setColor(QPalette::Button, QColor(100, 100, 100));
            _checkTCPServerButton->setPalette(pal);
        });

        _checkUDPServerButton = new QPushButton("Check UDP server", q);
        _checkUDPServerButton->setFixedSize(100, 30);
        _checkUDPServerButton->setAutoFillBackground(true);

        connect(_checkUDPServerButton, &QPushButton::pressed, this, [this]()
        {
            QPalette pal = _checkUDPServerButton->palette();
            pal.setColor(QPalette::Button, QColor(100, 100, 100));
            _checkUDPServerButton->setPalette(pal);
        });

        _checkICMPServerButton = new QPushButton("Check ICMP server", q);
        _checkICMPServerButton->setFixedSize(100, 30);
        _checkICMPServerButton->setAutoFillBackground(true);

        connect(_checkICMPServerButton, &QPushButton::pressed, this, [this]()
        {
            QPalette pal = _checkICMPServerButton->palette();
            pal.setColor(QPalette::Button, QColor(100, 100, 100));
            _checkICMPServerButton->setPalette(pal);
        });
    }

    void createServerMonitors()
    {
        Q_Q(MonitoringToolWidget);

        QString addr("google.com");
//         QString addr("127.0.0.1");
//         QString addr("64.233.165.139");
//         QString addr("http://example.com");

        _tcpServerMonitor = new TCPServerMonitor("TCP Host", addr, 80, q);
        _udpServerMonitor = new UDPServerMonitor("UDP Host", addr, 80, q);
        _icmpServerMonitor = new ICMPServerMonitor("ICMP Host", addr, 80, q);

        connect(_tcpServerMonitor, &ServerMonitor::succeeded, this, [this]()
        {
            QPalette pal = _checkTCPServerButton->palette();
            pal.setColor(QPalette::Button, QColor(100, 200, 100));
            _checkTCPServerButton->setPalette(pal);
        });
        connect(_tcpServerMonitor, &ServerMonitor::failed, this, [this]()
        {
            QPalette pal = _checkTCPServerButton->palette();
            pal.setColor(QPalette::Button, QColor(200, 100, 100));
            _checkTCPServerButton->setPalette(pal);
        });

        connect(_udpServerMonitor, &ServerMonitor::succeeded, this, [this]()
        {
            QPalette pal = _checkUDPServerButton->palette();
            pal.setColor(QPalette::Button, QColor(100, 200, 100));
            _checkUDPServerButton->setPalette(pal);
        });
        connect(_udpServerMonitor, &ServerMonitor::failed, this, [this]()
        {
            QPalette pal = _checkUDPServerButton->palette();
            pal.setColor(QPalette::Button, QColor(200, 100, 100));
            _checkUDPServerButton->setPalette(pal);
        });


        connect(_icmpServerMonitor, &ServerMonitor::succeeded, this, [this]()
        {
            QPalette pal = _checkICMPServerButton->palette();
            pal.setColor(QPalette::Button, QColor(100, 200, 100));
            _checkICMPServerButton->setPalette(pal);
        });
        connect(_icmpServerMonitor, &ServerMonitor::failed, this, [this]()
        {
            QPalette pal = _checkICMPServerButton->palette();
            pal.setColor(QPalette::Button, QColor(200, 100, 100));
            _checkICMPServerButton->setPalette(pal);
        });
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

        q->setLayout(mainLayout);
    }
};


ui::MonitoringToolWidget::MonitoringToolWidget(QWidget *parent)
    : QWidget(parent)
	, d_ptr(new MonitoringToolWidgetPrivate(this))
{
	Q_D(MonitoringToolWidget);

	setMinimumSize(200, 100);

	d->createServerMonitors();

	d->createCheckServerButtons();
	d->createLayouts();

    connect(d->_checkTCPServerButton, &QPushButton::clicked, d->_tcpServerMonitor, &ServerMonitor::checkServer);
    connect(d->_checkUDPServerButton, &QPushButton::clicked, d->_udpServerMonitor, &ServerMonitor::checkServer);
    connect(d->_checkICMPServerButton, &QPushButton::clicked, d->_icmpServerMonitor, &ServerMonitor::checkServer);
}

ui::MonitoringToolWidget::~MonitoringToolWidget()
{

}
