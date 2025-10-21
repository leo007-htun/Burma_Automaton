#ifndef BURMA_LOGGER_H
#define BURMA_LOGGER_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <QFile>
#include <QTextStream>

namespace Burma {

class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger& instance();

    void initialize(const QString &logFilePath = QString());
    void shutdown();

    void debug(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);

signals:
    void messageLogged(const QString &message, const QString &level);

private:
    Logger();
    ~Logger() override;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(const QString &message, const QString &level);

    QMutex m_mutex;
    QFile *m_logFile;
    QTextStream *m_logStream;
    bool m_initialized;
};

} // namespace Burma

#endif // BURMA_LOGGER_H
