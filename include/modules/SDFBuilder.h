#ifndef BURMA_SDFBUILDER_H
#define BURMA_SDFBUILDER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

namespace Burma {

/**
 * @brief Builds SDF world files from JSON world plans
 *
 * Converts LLM-generated world plans into valid Gazebo SDF format
 */
class SDFBuilder : public QObject
{
    Q_OBJECT

public:
    explicit SDFBuilder(QObject *parent = nullptr);
    ~SDFBuilder() override = default;

    // Build SDF from JSON world plan
    QString buildWorldSDF(const QJsonObject &worldPlan);

    // Save SDF to file
    bool saveToFile(const QString &sdfContent, const QString &filePath);

    // Generate SDF for individual model
    QString buildModelSDF(const QJsonObject &modelData);

signals:
    void buildComplete(const QString &sdfFilePath);
    void buildError(const QString &error);

private:
    QString generateWorldHeader(const QString &worldName);
    QString generatePhysicsSection(const QJsonObject &physicsData);
    QString generateLightingSection(const QJsonArray &lights);
    QString generateModelsSection(const QJsonArray &models);
    QString generateModelElement(const QJsonObject &model);

    QString generatePoseElement(const QJsonObject &position, const QJsonObject &rotation);
    QString generateScaleElement(const QJsonObject &scale);
    QString generateVisualElement(const QJsonObject &model);
    QString generateCollisionElement(const QJsonObject &model);

    QString escapeXML(const QString &text);
};

} // namespace Burma

#endif // BURMA_SDFBUILDER_H
