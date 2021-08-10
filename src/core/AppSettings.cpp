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

    const QString _defaultBackgroundColor = "dddddd";
    const QString _defaultGreenColor = "5bb749";
    const QString _defaultYellowColor = "fce23a";
    const QString _defaultRedColor = "d6415f";

public:
    explicit AppSettingsPrivate(AppSettings *q)
        : q_ptr(q)
    {
    }

    ~AppSettingsPrivate() = default;

    QColor readColor(const QSettings &settings, const QString &colorParameter, const QString &defaultColor)
    {
        auto strColor = settings.value(colorParameter, defaultColor).toString();
        auto colorTemplate = QString("#%1");

        if (strColor.size() != 6)
        {
            Logger::instance()->addLog(
                QString("Could not read color, value \"%1\" is invalid. Using default color \"%2\"")
                .arg(strColor)
                .arg(colorTemplate.arg(defaultColor)));
            strColor = defaultColor;
        }

        return QColor(colorTemplate.arg(strColor));
    }

    void readSettings()
    {
        QSettings settings(_filename, QSettings::IniFormat);

        settings.beginGroup("colors");
        _config.backgroundColor = readColor(settings, "backgroundColor", _defaultBackgroundColor);
        _config.greenColor = readColor(settings, "greenColor", _defaultGreenColor);
        _config.yellowColor = readColor(settings, "yellowColor", _defaultYellowColor);
        _config.redColor = readColor(settings, "redColor", _defaultRedColor);
        settings.endGroup();

        settings.beginGroup("polling");
        auto userFrequency = settings.value("frequencySeconds", _defaultFrequency).toInt();
        if (userFrequency <= 0)
        {
            Logger::instance()->addLog(
                QString("\"frequencySeconds\" parameter could be invalid, setting default value \"%1\"")
                .arg(_defaultFrequency));
            _config.frequencySeconds = _defaultFrequency;
        }
        else
            _config.frequencySeconds = userFrequency;

        auto userSensitivity = settings.value("sensitivity", _defaultSensitivity).toInt();

        if (userSensitivity < 0)
        {
            Logger::instance()->addLog(
                QString("\"sensitivity\" parameter could be invalid, setting default value \"%1\"")
                .arg(_defaultSensitivity));
            _config.sensitivity = _defaultSensitivity;
        }
        else
            _config.sensitivity = userSensitivity;

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
