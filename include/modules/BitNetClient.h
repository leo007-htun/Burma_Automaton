#ifndef BURMA_BITNETCLIENT_H
#define BURMA_BITNETCLIENT_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QJsonObject>

namespace Burma {

/**
 * @brief Client for communicating with BitNet.cpp LLM
 *
 * Handles natural language processing and world plan generation using
 * Microsoft's BitNet.cpp for efficient 1.58-bit inference
 */
class BitNetClient : public QObject
{
    Q_OBJECT

public:
    explicit BitNetClient(QObject *parent = nullptr);
    ~BitNetClient() override;

    // BitNet.cpp configuration
    void setBitNetPath(const QString &path);
    void setModelPath(const QString &path);

    // Process user prompt
    void processPrompt(const QString &prompt, const QStringList &context = QStringList());

    // Check if client is ready
    bool isReady() const { return m_isReady; }

signals:
    void worldPlanGenerated(const QJsonObject &worldPlan);
    void errorOccurred(const QString &error);
    void processingStarted();
    void processingFinished();

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    void runInference(const QString &prompt);
    QJsonObject parseResponse(const QString &output);
    QString createWorldPlanPrompt(const QString &prompt, const QStringList &context);
    QJsonObject createFallbackWorld();  // Create simple default world when BitNet fails

    QProcess *m_process;
    QString m_bitnetCliPath;
    QString m_modelPath;
    bool m_isReady;
};

} // namespace Burma

#endif // BURMA_BITNETCLIENT_H
