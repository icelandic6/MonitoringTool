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
        ~AppSettings();

        void readFile(const QString &iniFileName);

        static AppSettings *instance();
        QList<ServerInfo> serversInfo();

    private:
        explicit AppSettings();

    private:
        QScopedPointer<AppSettingsPrivate> d_ptr;
        Q_DISABLE_COPY(AppSettings)
        Q_DECLARE_PRIVATE(AppSettings)
    };
}