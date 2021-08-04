#include "MonitoringTool.h"
#include "ServersManager.h"
#include "ServerStatus.h"
#include "AppSettings.h"

#include "ui/MonitoringToolWidget.h"

#include <QApplication>
#include <QDebug>

class core::MonitoringToolPrivate : public QObject
{
    Q_DECLARE_PUBLIC(MonitoringTool)
    Q_DISABLE_COPY(MonitoringToolPrivate)
    MonitoringTool *q_ptr = nullptr;

    ui::MonitoringToolWidget *_monitoringWidget = nullptr;

    ServersManager *_serversManager = nullptr;

    QString _settingsFileName = QString("radhud.ini");

    QString _icmpPortTag = "ICMP";
    QString _udpPortTag = "*";

public:
    explicit MonitoringToolPrivate(MonitoringTool *q)
        : q_ptr(q)
    {
    }

    ~MonitoringToolPrivate() = default;

private:
    void createMonitorsManager()
    {
        _serversManager = new ServersManager(this);
    }

    void createMonitorWidget(const QMap<ushort, QString> &info)
    {
        _monitoringWidget = new ui::MonitoringToolWidget(info);
        _monitoringWidget->show();
        _monitoringWidget->setTrayServerStatus(info.first(), core::ServerStatus::Available);
    }

public:
    void minimizeApp()
    {
        _monitoringWidget->setWindowState(Qt::WindowMinimized);
    }

    void updateTrayIcon(const QString &name, core::ServerStatus status)
    {
        _monitoringWidget->setTrayServerStatus(name, status);
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

    auto settings = AppSettings::instance();
    settings->readFile(d->_settingsFileName);
    
    auto servers = settings->serversInfo();
    QMap<ushort, QString> idNames;

    for (auto si : servers)
    {
        ushort id;
        if (si.port == d->_icmpPortTag)
            id = d->_serversManager->addICMPServer(si.name, si.addr);
        else if (si.port.contains(d->_udpPortTag))
            id = d->_serversManager->addUDPServer(si.name, si.addr, si.port.remove(d->_udpPortTag).toInt());
        else
            id = d->_serversManager->addTCPServer(si.name, si.addr, si.port.toInt());

        idNames[id] = si.name;
    }

    d->createMonitorWidget(idNames);

    connect(d->_monitoringWidget, &ui::MonitoringToolWidget::closeApp, d, &MonitoringToolPrivate::closeApp);
    connect(d->_serversManager, &ServersManager::serverStatusUpdated, this, [this](ushort serverId, ServerStatus status)
    {
        Q_D(MonitoringTool);

        //TODO: remove this lambda after debugging
        d->_monitoringWidget->setServerStatus(serverId, status);

        QString s = (status == ServerStatus::Available) ? "AVAILABLE" : ((status == ServerStatus::Unstable) ? "UNSTABLE" : "FAILED");

        qDebug() << "====== MonitoringTool: server [" << serverId << "] status updated to " << s << "]\n";
    });
    connect(d->_serversManager, &ServersManager::worstServerUpdated, d, &MonitoringToolPrivate::updateTrayIcon);

    d->_serversManager->startMonitoring();
}

core::MonitoringTool::~MonitoringTool()
{
    Q_D(MonitoringTool);

    if (d->_monitoringWidget)
        delete d->_monitoringWidget;
}
