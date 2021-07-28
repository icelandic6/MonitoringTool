#include "MonitoringTool.h"
#include "ServerMonitorsManager.h"

#include "ui/MonitoringToolWidget.h"

#include "net/TCPServerMonitor.h"
#include "net/UDPServerMonitor.h"
#include "net/ICMPServerMonitor.h"

#include <QApplication>

class core::MonitoringToolPrivate : public QObject
{
    Q_DECLARE_PUBLIC(MonitoringTool)
    Q_DISABLE_COPY(MonitoringToolPrivate)
    MonitoringTool *q_ptr = nullptr;

    ui::MonitoringToolWidget *_mtoolWidget = nullptr;

    ServerMonitor *_tcpServerMonitor = nullptr;
    ServerMonitor *_udpServerMonitor = nullptr;
    ServerMonitor *_icmpServerMonitor = nullptr;

    ServerMonitorsManager *_monitorsManager = nullptr;

public:
    explicit MonitoringToolPrivate(MonitoringTool *q)
        : q_ptr(q)
    {
    }

    ~MonitoringToolPrivate()
    {
    }

private:
    void createServerMonitors()
    {
        Q_Q(MonitoringTool);

//         QString addr("google.com");
//         QString addr("127.0.0.1");
        QString addr("64.233.165.139");
//         QString addr("http://example.com");

        _tcpServerMonitor = new TCPServerMonitor("TCP Host", addr, 80, q);
        _udpServerMonitor = new UDPServerMonitor("UDP Host", addr, 80, q);
        _icmpServerMonitor = new ICMPServerMonitor("ICMP Host", addr, q);

        _monitorsManager->addServerMonitor(_tcpServerMonitor);
        _monitorsManager->addServerMonitor(_udpServerMonitor);
        _monitorsManager->addServerMonitor(_icmpServerMonitor);

//         connect(_tcpServerMonitor, &ServerMonitor::succeeded, this, [this]()
//         {
//             QPalette pal = _checkTCPServerButton->palette();
//             pal.setColor(QPalette::Button, QColor(100, 200, 100));
//             _checkTCPServerButton->setPalette(pal);
//         });
//         connect(_tcpServerMonitor, &ServerMonitor::failed, this, [this]()
//         {
//             QPalette pal = _checkTCPServerButton->palette();
//             pal.setColor(QPalette::Button, QColor(200, 100, 100));
//             _checkTCPServerButton->setPalette(pal);
//         });
// 
//         connect(_udpServerMonitor, &ServerMonitor::succeeded, this, [this]()
//         {
//             QPalette pal = _checkUDPServerButton->palette();
//             pal.setColor(QPalette::Button, QColor(100, 200, 100));
//             _checkUDPServerButton->setPalette(pal);
//         });
//         connect(_udpServerMonitor, &ServerMonitor::failed, this, [this]()
//         {
//             QPalette pal = _checkUDPServerButton->palette();
//             pal.setColor(QPalette::Button, QColor(200, 100, 100));
//             _checkUDPServerButton->setPalette(pal);
//         });
// 
//         connect(_icmpServerMonitor, &ServerMonitor::succeeded, this, [this]()
//         {
//             QPalette pal = _checkICMPServerButton->palette();
//             pal.setColor(QPalette::Button, QColor(100, 200, 100));
//             _checkICMPServerButton->setPalette(pal);
//         });
//         connect(_icmpServerMonitor, &ServerMonitor::failed, this, [this]()
//         {
//             QPalette pal = _checkICMPServerButton->palette();
//             pal.setColor(QPalette::Button, QColor(200, 100, 100));
//             _checkICMPServerButton->setPalette(pal);
//         });
    }

    void createMonitorsManager()
    {
        _monitorsManager = new ServerMonitorsManager(this);
    }

public:
    void minimizeApp()
    {
        _mtoolWidget->setWindowState(Qt::WindowMinimized);
    }

    void closeApp()
    {
        qApp->exit();
    }
};


core::MonitoringTool::MonitoringTool(QObject *parent)
    : QObject(parent)
    , d_ptr(new MonitoringToolPrivate(this))
{
    Q_D(MonitoringTool);

    d->createMonitorsManager();
    d->createServerMonitors();

    d->_mtoolWidget = new ui::MonitoringToolWidget();
    d->_mtoolWidget->show();

    connect(d->_mtoolWidget, &ui::MonitoringToolWidget::checkTCPServer, d->_tcpServerMonitor, &ServerMonitor::checkServer);
    connect(d->_mtoolWidget, &ui::MonitoringToolWidget::checkUDPServer, d->_udpServerMonitor, &ServerMonitor::checkServer);
    connect(d->_mtoolWidget, &ui::MonitoringToolWidget::checkICMPServer, d->_icmpServerMonitor, &ServerMonitor::checkServer);
}

core::MonitoringTool::~MonitoringTool() = default;
