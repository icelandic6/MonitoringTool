#include "Logger.h"

#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class core::LoggerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(Logger)
    Q_DISABLE_COPY(LoggerPrivate)
    Logger *q_ptr = nullptr;

    QStringList _logs;

    const short _maxLogs = 50;

public:
    explicit LoggerPrivate(Logger *q)
        : q_ptr(q)
    {
    }

    ~LoggerPrivate() = default;
};

core::Logger::Logger()
    : d_ptr(new LoggerPrivate(this))
{
}

core::Logger::~Logger() = default;

core::Logger* core::Logger::instance()
{
    static Logger *_instance = nullptr;
    if (_instance == nullptr)
        _instance = new Logger();

    return _instance;
}

void core::Logger::addLog(const QString &message)
{
    Q_D(Logger);

    auto fullMessage = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
        + ": "
        + message;

    if (d->_logs.size() >= d->_maxLogs)
        d->_logs.removeFirst();

    d->_logs.append(fullMessage);
    emit logAdded(fullMessage);
}

QStringList core::Logger::logs() const
{
    Q_D(const Logger);

    return d->_logs;
}
