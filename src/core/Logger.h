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

        void addLog(const QString &message);
        QStringList logs() const;

    signals:
        void logAdded(const QString &message);

    private:
        Logger();

    private:
        QScopedPointer<LoggerPrivate> d_ptr;
        Q_DISABLE_COPY(Logger)
        Q_DECLARE_PRIVATE(Logger)
    };
}