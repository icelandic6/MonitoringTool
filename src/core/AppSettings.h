#pragma once

#include <QObject>
#include <QColor>

namespace core
{
    static QString icmpPortTag = "ICMP";
    static QString udpPortTag = "*";

    struct ServerInfo
    {
        QString name;
        QString addr;
        QString port;
    };

    class AppSettingsPrivate;
    class AppSettings : public QObject
    {
    public:
        ~AppSettings();

        static AppSettings *instance();

        void readSettings();

        QColor backgroundColor() const;
        QColor greenColor() const;
        QColor yellowColor() const;
        QColor redColor() const;

        int frequencySeconds() const;
        int sensitivity() const;

        QList<ServerInfo> serversInfo() const;

    private:
        AppSettings();

    private:
        QScopedPointer<AppSettingsPrivate> d_ptr;
        Q_DISABLE_COPY(AppSettings)
        Q_DECLARE_PRIVATE(AppSettings)
    };
}