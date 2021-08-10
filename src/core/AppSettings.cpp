#include "AppSettings.h"
#include "Logger.h"

#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>

struct AppConfig
{
    QColor backgroundColor;
    QColor greenColor;
    QColor yellowColor;
    QColor redColor;

    int frequencySeconds;
    int sensitivity;
};

class core::AppSettingsPrivate : public QObject
{
    Q_DECLARE_PUBLIC(AppSettings)
    Q_DISABLE_COPY(AppSettingsPrivate)
    AppSettings *q_ptr = nullptr;

    QString _filename;
    AppConfig _config;

    const int _defaultFrequency = 15;
    const int _defaultSensitivity = 3;

public:
    explicit AppSettingsPrivate(AppSettings *q)
        : q_ptr(q)
    {
    }

    ~AppSettingsPrivate() = default;

    void readSettings()
    {
        QSettings settings(_filename, QSettings::IniFormat);

        settings.beginGroup("colors");
        _config.backgroundColor = QColor(QString("#%1")
            .arg(settings.value("backgroundColor", "fafafa").toString()));
        _config.greenColor = QColor(QString("#%1")
            .arg(settings.value("greenColor", "5bb749").toString()));
        _config.yellowColor = QColor(QString("#%1")
            .arg(settings.value("yellowColor", "fce23a").toString()));
        _config.redColor = QColor(QString("#%1")
            .arg(settings.value("redColor", "d6415f").toString()));
        settings.endGroup();

        settings.beginGroup("polling");
        auto userFrequency = settings.value("frequencySeconds", _defaultFrequency).toInt();
        _config.frequencySeconds = std::max(userFrequency, 1);
        auto userSensitivity = settings.value("sensitivity", _defaultSensitivity).toInt();
        _config.sensitivity = std::max(userSensitivity, 0);
        settings.endGroup();
    }

    QList<ServerInfo> readServersInfo() const
    {
        QList<ServerInfo> servers;

        QFile f(_filename);
        if (f.open(QIODevice::ReadOnly))
        {
            QTextStream inStream(&f);
            bool resources = false;

            while (!inStream.atEnd())
            {
                auto line = inStream.readLine();
                if (line == "[resources]")
                {
                    resources = true;
                    continue;
                }

                if (!resources)
                    continue;

                QStringList addr = line.simplified().split(" ");
                if (addr.size() == 1 && addr.first().isEmpty())
                    continue;

                if (addr.size() != 3)
                {
                    Logger::instance()->addLog(QString("Could not read line \"%1\"").arg(line));
                    continue;
                }

                ServerInfo si;
                si.name = addr[0];
                si.addr = addr[1];
                si.port = addr[2];
                servers.append(si);
            }

            f.close();
        }
        else
        {
            QMessageBox::critical(nullptr, QString("Monitoring Tool"),
                QString("No %1 file found").arg(_filename),
                QMessageBox::Ok, QMessageBox::Ok);
            exit(EXIT_FAILURE);
        }

        return servers;
    }
};

core::AppSettings::AppSettings()
    : d_ptr(new AppSettingsPrivate(this))
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, &AppSettings::deleteLater);
}

core::AppSettings::~AppSettings() = default;

void core::AppSettings::readFile(const QString &iniFileName)
{
    Q_D(AppSettings);

    d->_filename = iniFileName;
    d->readSettings();
}

QColor core::AppSettings::backgroundColor() const
{
    Q_D(const AppSettings);

    return d->_config.backgroundColor;
}

QColor core::AppSettings::greenColor() const
{
    Q_D(const AppSettings);

    return d->_config.greenColor;
}

QColor core::AppSettings::yellowColor() const
{
    Q_D(const AppSettings);

    return d->_config.yellowColor;
}

QColor core::AppSettings::redColor() const
{
    Q_D(const AppSettings);

    return d->_config.redColor;
}

int core::AppSettings::frequencySeconds() const
{
    Q_D(const AppSettings);

    return d->_config.frequencySeconds;
}

int core::AppSettings::sensitivity() const
{
    Q_D(const AppSettings);

    return d->_config.sensitivity;
}

core::AppSettings* core::AppSettings::instance()
{
    static AppSettings *_instance = nullptr;
    if (_instance == nullptr)
        _instance = new AppSettings();

    return _instance;
}

QList<core::ServerInfo> core::AppSettings::serversInfo() const
{
    Q_D(const AppSettings);

    return d->readServersInfo();
}
