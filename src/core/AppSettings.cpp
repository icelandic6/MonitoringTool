#include "AppSettings.h"

#include <QSettings>
#include <QColor>
#include <QFile>
#include <QMessageBox>
#include <QApplication>

#include <QDebug>


struct IniConfig
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
    IniConfig _config;

    int _defaultFrequency = 15;
    int _defaultSensitivity = 3;

public:
    explicit AppSettingsPrivate(AppSettings *q)
        : q_ptr(q)
    {
    }

    ~AppSettingsPrivate() = default;

    void readSettings()
    {
        QSettings settings(_filename, QSettings::IniFormat);

        qDebug() << "==== ALL SETTINGS: \n" << settings.allKeys();
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
        _config.frequencySeconds = settings.value("frequencySeconds", _defaultFrequency).toInt();
        _config.sensitivity = settings.value("sensitivity", _defaultFrequency).toInt();
        settings.endGroup();
    }

    QList<ServerInfo> readServersInfo()
    {
        QList<ServerInfo> servers;

        QFile f(_filename);
        if (f.open(QIODevice::ReadOnly))
        {

            QTextStream in(&f);
            bool resources = false;
            bool failed = false;
            while (!in.atEnd())
            {
                QString line = in.readLine();
                if (line == "[resources]")
                {
                    resources = true;
                    continue;
                }

                if (!resources)
                    continue;

                QStringList addr = line.simplified().split(" ");
                if (addr.size() != 3)
                {
                    failed = true;
                    continue;
                }

                ServerInfo si;
                si.name = addr[0];
                si.addr = addr[1];
                si.port = addr[2];
                servers.append(si);
            }

            f.close();
            if (failed)
            {
                QMessageBox::warning(nullptr, QString("Monitoring Tool"),
                    QString("Could not read some servers"),
                    QMessageBox::Ok, QMessageBox::Ok);
            }
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


core::AppSettings::AppSettings(const QString &iniFileName)
    : d_ptr(new AppSettingsPrivate(this))
{
    Q_D(AppSettings);

    d->_filename = iniFileName;
    d->readSettings();
}

core::AppSettings::~AppSettings() = default;

QList<core::ServerInfo> core::AppSettings::serversInfo()
{
    Q_D(AppSettings);

    return d->readServersInfo();
}