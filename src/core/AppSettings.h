#pragma once

#include <QObject>
#include <QColor>

namespace core
{
    struct AppConfig
    {
        QColor backgroundColor;
        QColor greenColor;
        QColor yellowColor;
        QColor redColor;

        int frequencySeconds;
        int sensitivity;
    };

    struct ServerInfo
    {
        QString name;
        QString addr;
        QString port;
    };

    class AppSettingsPrivate;
    class AppSettings : public QObject
    {
        Q_OBJECT
    public:
        ~AppSettings();

        void readFile(const QString &iniFileName);

        const AppConfig &config() const;

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