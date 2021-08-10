#include "MonitoringTool.h"
#include "ServersManager.h"
#include "ServerStatus.h"
#include "AppSettings.h"
#include "Logger.h"

#include "ui/MonitoringToolWidget.h"

#include <QApplication>

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

        if (!info.isEmpty())
            _monitoringWidget->setTrayServerStatus(info.keys().first(), core::ServerStatus::Available);
        else
            Logger::instance()->addLog(QString("Servers list is empty"));
    }

public:
    void updateServerStatus(ushort id, core::ServerStatus status)
    {
        _monitoringWidget->setServerStatus(id, status);
    }

    void updateServerLatency(ushort id, int latency)
    {
        _monitoringWidget->setServerLatency(id, latency);
    }

    void updateTrayIconStatus(ushort id, core::ServerStatus status)
    {
        _monitoringWidget->setTrayServerStatus(id, status);
    }

    void updateTrayIconLatency(ushort id, int latency)
    {
        _monitoringWidget->setTrayServerLatency(id, latency);
    }

    void minimizeApp()
    {
        _monitoringWidget->setWindowState(Qt::WindowMinimized);
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
    connect(d->_serversManager, &ServersManager::serverStatusUpdated, d, &MonitoringToolPrivate::updateServerStatus);
    connect(d->_serversManager, &ServersManager::worstServerUpdated, d, &MonitoringToolPrivate::updateTrayIconStatus);
    connect(d->_serversManager, &ServersManager::serverLatencyUpdated, d, &MonitoringToolPrivate::updateServerLatency);
    connect(d->_serversManager, &ServersManager::serverLatencyUpdated, d, &MonitoringToolPrivate::updateTrayIconLatency);

    d->_serversManager->startMonitoring();
}

core::MonitoringTool::~MonitoringTool()
{
    Q_D(MonitoringTool);

    if (d->_monitoringWidget)
        delete d->_monitoringWidget;
}
