#ifndef BURMA_FUELFETCHER_H
#define BURMA_FUELFETCHER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>

namespace Burma {

/**
 * @brief Fetches and downloads models from Gazebo Fuel
 *
 * Searches for existing assets and downloads them when needed
 */
class FuelFetcher : public QObject
{
    Q_OBJECT

public:
    explicit FuelFetcher(QObject *parent = nullptr);
    ~FuelFetcher() override;

    // Search for models
    void searchModels(const QString &query);

    // Download a specific model
    void downloadModel(const QString &modelUrl, const QString &destinationPath);

    // Check if model exists locally
    bool isModelCached(const QString &modelName) const;

    // Get local model path
    QString getLocalModelPath(const QString &modelName) const;

signals:
    void searchResultsReady(const QJsonArray &results);
    void downloadProgress(int percentage);
    void downloadComplete(const QString &modelPath);
    void errorOccurred(const QString &error);

private slots:
    void onSearchFinished();
    void onDownloadFinished();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QString getCacheDirectory() const;
    void ensureCacheDirectory();

    QNetworkAccessManager *m_networkManager;
    QString m_fuelApiBase;
    QString m_cacheDirectory;
};

} // namespace Burma

#endif // BURMA_FUELFETCHER_H
