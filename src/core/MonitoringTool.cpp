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

    ui::MonitoringToolWidget *_monitoringWidget = nullptr;

    ServerMonitor *_tcpServerMonitor = nullptr;
    ServerMonitor *_udpServerMonitor = nullptr;
    ServerMonitor *_icmpServerMonitor = nullptr;

    ServersManager *_serversManager = nullptr;

public:
    explicit MonitoringToolPrivate(MonitoringTool *q)
        : q_ptr(q)
    {
    }

    ~MonitoringToolPrivate() = default;

private:
    void createMonitorsManager()
    {
        _serversManager = new ServersManager(10000, 3, this);
    }

public:
    void minimizeApp()
    {
        _monitoringWidget->setWindowState(Qt::WindowMinimized);
    }

    void closeApp()
    {
        qApp->exit();
    }

    void updateTrayIcon(const QString &name, core::ServerStatus status)
    {
        _monitoringWidget->setTrayServerStatus(name, status);
    }
};


core::MonitoringTool::MonitoringTool(QObject *parent)
    : QObject(parent)
    , d_ptr(new MonitoringToolPrivate(this))
{
    Q_D(MonitoringTool);

    d->createMonitorsManager();

//     QString addr("google.com");
//     QString addr("127.0.0.1");
    QString valid_addr("64.233.165.139");
    QString invalid_addr("69.69.69.69");
//     QString addr("http://example.com");

    QMap<ushort, QString> serversInfo;

    QString name = "TCP Host 1";
    ushort id = d->_serversManager->addTCPServer(name, valid_addr, 80);
    serversInfo[id] = name;

    name = "TCP Host 2";
    id = d->_serversManager->addTCPServer(name, invalid_addr, 80);
    serversInfo[id] = name;

    name = "UDP Host";
    id = d->_serversManager->addUDPServer(name, valid_addr, 80);
    serversInfo[id] = name;

    name = "ICMP Host";
    id = d->_serversManager->addICMPServer(name, valid_addr);
    serversInfo[id] = name;

    name = "ICMP Host 2";
    id = d->_serversManager->addICMPServer(name, valid_addr);
    serversInfo[id] = name;

    name = "ICMP Host 3";
    id = d->_serversManager->addICMPServer(name, valid_addr);
    serversInfo[id] = name;

//     name = "ICMP Host 4";
//     id = d->_serversManager->addICMPServer(name, valid_addr);
//     serversInfo[id] = name;

//     name = "ICMP Host 5";
//     id = d->_serversManager->addICMPServer(name, valid_addr);
//     serversInfo[id] = name;

    d->_monitoringWidget = new ui::MonitoringToolWidget(serversInfo);
    d->_monitoringWidget->show();
    d->_monitoringWidget->setTrayServerStatus(name, core::ServerStatus::Available);

    connect(d->_monitoringWidget, &ui::MonitoringToolWidget::closeApp, this, [this]()
    {
        qApp->exit();
    });

    connect(d->_serversManager, &ServersManager::serverStatusUpdated, this, [this](ushort serverId, ServerStatus status)
    {
        Q_D(MonitoringTool);

        //TODO: remove this lambda after debugging
        d->_monitoringWidget->setServerStatus(serverId, status);

        QString s = (status == ServerStatus::Available) ? "AVAILABLE" : ((status == ServerStatus::Unstable) ? "UNSTABLE" : "FAILED");

        qDebug() << "====== MonitoringTool: server [" << serverId << "] status updated to " << s << "]\n";
    });
    connect(d->_serversManager, &ServersManager::worstServerUpdated,
            d, &MonitoringToolPrivate::updateTrayIcon);
}

core::MonitoringTool::~MonitoringTool()
{
    Q_D(MonitoringTool);

    if (d->_monitoringWidget)
        delete d->_monitoringWidget;
}
