#pragma once

#include <QObject>

namespace core
{
    struct ServerInfo
    {
        QString name;
        QString addr;
        QString port;
    };

    static QString IcmpPort = "ICMP";
    static QString UdpPort = "*";

    class AppSettingsPrivate;
    class AppSettings : public QObject
    {
        Q_OBJECT
    public:
        explicit AppSettings(const QString &iniFileName);
        ~AppSettings();

//         static AppSettings *instance();
        QList<ServerInfo> serversInfo();

    private:
        QScopedPointer<AppSettingsPrivate> d_ptr;
        Q_DISABLE_COPY(AppSettings)
        Q_DECLARE_PRIVATE(AppSettings)
    };
}