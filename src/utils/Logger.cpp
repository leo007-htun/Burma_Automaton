#include "utils/Logger.h"
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <iostream>

namespace Burma {

Logger::Logger()
    : m_logFile(nullptr)
    , m_logStream(nullptr)
    , m_initialized(false)
{
}

Logger::~Logger()
{
    shutdown();
}

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

void Logger::initialize(const QString &logFilePath)
{
    QMutexLocker locker(&m_mutex);

    if (m_initialized) {
        return;
    }

    QString logPath = logFilePath;
    if (logPath.isEmpty()) {
        // Default log location
        QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(appDataPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        logPath = dir.filePath("burma_automaton.log");
    }

    m_logFile = new QFile(logPath);
    if (m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        m_logStream = new QTextStream(m_logFile);
        m_initialized = true;

        QString startMsg = QString("=== Burma Automaton Log Started: %1 ===")
            .arg(QDateTime::currentDateTime().toString(Qt::ISODate));
        *m_logStream << startMsg << "\n";
        m_logStream->flush();

        std::cout << startMsg.toStdString() << std::endl;
    }
}

void Logger::shutdown()
{
    QMutexLocker locker(&m_mutex);

    if (!m_initialized) {
        return;
    }

    if (m_logStream) {
        QString endMsg = QString("=== Burma Automaton Log Ended: %1 ===")
            .arg(QDateTime::currentDateTime().toString(Qt::ISODate));
        *m_logStream << endMsg << "\n";
        m_logStream->flush();

        delete m_logStream;
        m_logStream = nullptr;
    }

    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }

    m_initialized = false;
}

void Logger::debug(const QString &message)
{
    log(message, "DEBUG");
}

void Logger::info(const QString &message)
{
    log(message, "INFO");
}

void Logger::warning(const QString &message)
{
    log(message, "WARNING");
}

void Logger::error(const QString &message)
{
    log(message, "ERROR");
}

void Logger::log(const QString &message, const QString &level)
{
    QMutexLocker locker(&m_mutex);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString logLine = QString("[%1] [%2] %3")
        .arg(timestamp)
        .arg(level.leftJustified(7))
        .arg(message);

    // Write to console
    if (level == "ERROR") {
        std::cerr << logLine.toStdString() << std::endl;
    } else {
        std::cout << logLine.toStdString() << std::endl;
    }

    // Write to file
    if (m_initialized && m_logStream) {
        *m_logStream << logLine << "\n";
        m_logStream->flush();
    }

    // Emit signal for UI
    emit messageLogged(message, level);
}

} // namespace Burma
