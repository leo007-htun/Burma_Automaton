#ifndef BURMA_MATERIALMANAGER_H
#define BURMA_MATERIALMANAGER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QMap>

namespace Burma {

/**
 * @brief Manages materials, textures, and colors for Gazebo models
 *
 * Applies visual properties dynamically to models
 * Reference: https://classic.gazebosim.org/tutorials?tut=color_model
 */
class MaterialManager : public QObject
{
    Q_OBJECT

public:
    explicit MaterialManager(QObject *parent = nullptr);
    ~MaterialManager() override = default;

    // Apply color to model
    void setModelColor(const QString &modelName, const QColor &color);

    // Apply material/texture
    void setModelMaterial(const QString &modelName, const QString &materialName);

    // Get available materials
    QStringList getAvailableMaterials() const;

    // Create custom material
    void createMaterial(const QString &name, const QColor &ambient,
                       const QColor &diffuse, const QColor &specular);

signals:
    void materialApplied(const QString &modelName, const QString &materialName);
    void colorApplied(const QString &modelName, const QColor &color);

private:
    struct Material {
        QString name;
        QColor ambient;
        QColor diffuse;
        QColor specular;
        QString texture;
    };

    void initializeDefaultMaterials();
    QString colorToString(const QColor &color);

    QMap<QString, Material> m_materials;
};

} // namespace Burma

#endif // BURMA_MATERIALMANAGER_H
