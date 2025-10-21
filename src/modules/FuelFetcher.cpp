#include "modules/FuelFetcher.h"
#include "utils/Logger.h"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

namespace Burma {

FuelFetcher::FuelFetcher(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_fuelApiBase("https://fuel.gazebosim.org/1.0")
{
    m_cacheDirectory = getCacheDirectory();
    ensureCacheDirectory();
}

FuelFetcher::~FuelFetcher()
{
}

QString FuelFetcher::getCacheDirectory() const
{
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    return QDir(cacheDir).filePath("gazebo_models");
}

void FuelFetcher::ensureCacheDirectory()
{
    QDir dir(m_cacheDirectory);
    if (!dir.exists()) {
        if (dir.mkpath(".")) {
            Logger::instance().info("Created model cache directory: " + m_cacheDirectory);
        } else {
            Logger::instance().error("Failed to create cache directory: " + m_cacheDirectory);
        }
    }
}

void FuelFetcher::searchModels(const QString &query)
{
    Logger::instance().info("Searching Gazebo Fuel for: " + query);

    // Construct search URL
    QString searchUrl = QString("%1/models?q=%2")
        .arg(m_fuelApiBase)
        .arg(QString(QUrl::toPercentEncoding(query)));

    QNetworkRequest request{QUrl(searchUrl)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &FuelFetcher::onSearchFinished);
}

void FuelFetcher::downloadModel(const QString &modelUrl, const QString &destinationPath)
{
    Logger::instance().info("Downloading model from: " + modelUrl);

    QNetworkRequest request{QUrl(modelUrl)};

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &FuelFetcher::onDownloadFinished);
    connect(reply, &QNetworkReply::downloadProgress, this, &FuelFetcher::onDownloadProgress);

    // Store destination path as property
    reply->setProperty("destinationPath", destinationPath);
}

bool FuelFetcher::isModelCached(const QString &modelName) const
{
    QString modelPath = getLocalModelPath(modelName);
    return QDir(modelPath).exists();
}

QString FuelFetcher::getLocalModelPath(const QString &modelName) const
{
    return QDir(m_cacheDirectory).filePath(modelName);
}

void FuelFetcher::onSearchFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        Logger::instance().debug("Search response: " + QString::fromUtf8(responseData));

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            QJsonArray results;

            if (doc.isArray()) {
                results = doc.array();
            } else if (doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj.contains("models")) {
                    results = obj["models"].toArray();
                }
            }

            Logger::instance().info(QString("Found %1 models").arg(results.size()));
            emit searchResultsReady(results);
        } else {
            QString error = "Failed to parse search results: " + parseError.errorString();
            Logger::instance().error(error);
            emit errorOccurred(error);
        }
    } else {
        QString error = "Search failed: " + reply->errorString();
        Logger::instance().error(error);
        emit errorOccurred(error);
    }

    reply->deleteLater();
}

void FuelFetcher::onDownloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        QString destinationPath = reply->property("destinationPath").toString();
        QByteArray modelData = reply->readAll();

        // Ensure destination directory exists
        QFileInfo fileInfo(destinationPath);
        QDir dir = fileInfo.dir();
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        // Save model file
        QFile file(destinationPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(modelData);
            file.close();

            Logger::instance().info("Model downloaded to: " + destinationPath);
            emit downloadComplete(destinationPath);
        } else {
            QString error = "Failed to save model: " + file.errorString();
            Logger::instance().error(error);
            emit errorOccurred(error);
        }
    } else {
        QString error = "Download failed: " + reply->errorString();
        Logger::instance().error(error);
        emit errorOccurred(error);
    }

    reply->deleteLater();
}

void FuelFetcher::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        int percentage = static_cast<int>((bytesReceived * 100) / bytesTotal);
        emit downloadProgress(percentage);
    }
}

} // namespace Burma
