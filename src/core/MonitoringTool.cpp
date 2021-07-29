#include "MonitoringTool.h"
#include "ServersManager.h"
#include "ServerStatus.h"

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

    ServersManager *_serversManager = nullptr;

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
        QString valid_addr("64.233.165.139");
        QString invalid_addr("69.69.69.69");
//         QString addr("http://example.com");

//         _tcpServerMonitor = new TCPServerMonitor("TCP Host", addr, 80, q);
//         _udpServerMonitor = new UDPServerMonitor("UDP Host", addr, 80, q);
//         _icmpServerMonitor = new ICMPServerMonitor("ICMP Host", "google.com", q);

//         _serversManager->addServerMonitor(_tcpServerMonitor);
//         _serversManager->addServerMonitor(_udpServerMonitor);
//         _serversManager->addServerMonitor(_icmpServerMonitor);

        _serversManager->addTCPServer("TCP Host", invalid_addr, 80);
//         _serversManager->addUDPServer("UDP Host", valid_addr, 80);
//         _serversManager->addICMPServer("ICMP Host", valid_addr);

        connect(_serversManager, &ServersManager::serverStatusUpdated, this, [this](uint serverId, ServerStatus status)
        {
            QString s = (status == ServerStatus::Available) ? "AVAILABLE" : ((status == ServerStatus::Unstable) ? "UNSTABLE" : "FAILED");

            qDebug() << "====== MonitoringTool: server [" << serverId << "] status updated to " << s << "]\n";
        });
    }

    void createMonitorsManager()
    {
        _serversManager = new ServersManager(10000, 3, this);
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

//     connect(d->_mtoolWidget, &ui::MonitoringToolWidget::checkTCPServer, d->_tcpServerMonitor, &ServerMonitor::checkServer);
//     connect(d->_mtoolWidget, &ui::MonitoringToolWidget::checkUDPServer, d->_udpServerMonitor, &ServerMonitor::checkServer);
//     connect(d->_mtoolWidget, &ui::MonitoringToolWidget::checkICMPServer, d->_icmpServerMonitor, &ServerMonitor::checkServer);
}

core::MonitoringTool::~MonitoringTool() = default;
