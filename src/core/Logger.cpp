#include "Logger.h"

#include <QSettings>
#include <QFile>
#include <QTextStream>

class core::LoggerPrivate : public QObject
{
    Q_DECLARE_PUBLIC(Logger)
    Q_DISABLE_COPY(LoggerPrivate)
    Logger *q_ptr = nullptr;

    QStringList _logs;

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

void core::Logger::addLog(const QString &message, bool critical)
{
    Q_D(Logger);

    d->_logs.append(message);
    emit logAdded(message, critical);
}

QStringList core::Logger::logs() const
{
    Q_D(const Logger);

    return d->_logs;
}
