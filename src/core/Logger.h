#pragma once

#include <QObject>

namespace core
{
    class LoggerPrivate;
    class Logger : public QObject
    {
        Q_OBJECT
    public:
        ~Logger();

        static Logger *instance();

        void addLog(const QString &message, bool critical = false);
        QStringList logs() const;

    signals:
        void logAdded(const QString &message, bool critical);

    private:
        Logger();

    private:
        QScopedPointer<LoggerPrivate> d_ptr;
        Q_DISABLE_COPY(Logger)
        Q_DECLARE_PRIVATE(Logger)
    };
}