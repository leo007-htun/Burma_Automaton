#include "modules/MaterialManager.h"
#include "utils/Logger.h"

namespace Burma {

MaterialManager::MaterialManager(QObject *parent)
    : QObject(parent)
{
    initializeDefaultMaterials();
}

void MaterialManager::initializeDefaultMaterials()
{
    // Create some default materials based on Gazebo's material system

    // Basic colors
    createMaterial("Red", QColor(255, 0, 0), QColor(255, 0, 0), QColor(255, 255, 255));
    createMaterial("Green", QColor(0, 255, 0), QColor(0, 255, 0), QColor(255, 255, 255));
    createMaterial("Blue", QColor(0, 0, 255), QColor(0, 0, 255), QColor(255, 255, 255));
    createMaterial("White", QColor(255, 255, 255), QColor(255, 255, 255), QColor(255, 255, 255));
    createMaterial("Black", QColor(0, 0, 0), QColor(50, 50, 50), QColor(255, 255, 255));
    createMaterial("Gray", QColor(128, 128, 128), QColor(128, 128, 128), QColor(255, 255, 255));

    // Metallic materials
    createMaterial("Steel", QColor(100, 100, 100), QColor(150, 150, 150), QColor(255, 255, 255));
    createMaterial("Aluminum", QColor(200, 200, 200), QColor(220, 220, 220), QColor(255, 255, 255));
    createMaterial("Gold", QColor(255, 215, 0), QColor(255, 215, 0), QColor(255, 255, 200));
    createMaterial("Copper", QColor(184, 115, 51), QColor(184, 115, 51), QColor(255, 200, 150));

    // Common materials
    createMaterial("Concrete", QColor(150, 150, 150), QColor(180, 180, 180), QColor(100, 100, 100));
    createMaterial("Wood", QColor(139, 90, 43), QColor(160, 110, 60), QColor(100, 100, 100));
    createMaterial("Plastic", QColor(200, 200, 200), QColor(220, 220, 220), QColor(255, 255, 255));
    createMaterial("Glass", QColor(200, 200, 255, 100), QColor(230, 230, 255, 100), QColor(255, 255, 255));

    Logger::instance().info(QString("Initialized %1 default materials").arg(m_materials.size()));
}

void MaterialManager::setModelColor(const QString &modelName, const QColor &color)
{
    Logger::instance().info(QString("Setting color for model %1: %2")
                          .arg(modelName)
                          .arg(color.name()));

    // TODO: Apply color to actual Gazebo model
    // This would interact with Gazebo's rendering system

    emit colorApplied(modelName, color);
}

void MaterialManager::setModelMaterial(const QString &modelName, const QString &materialName)
{
    if (!m_materials.contains(materialName)) {
        Logger::instance().warning("Material not found: " + materialName);
        return;
    }

    Logger::instance().info(QString("Applying material %1 to model %2")
                          .arg(materialName)
                          .arg(modelName));

    // TODO: Apply material to actual Gazebo model
    // This would interact with Gazebo's rendering system

    emit materialApplied(modelName, materialName);
}

QStringList MaterialManager::getAvailableMaterials() const
{
    return m_materials.keys();
}

void MaterialManager::createMaterial(const QString &name, const QColor &ambient,
                                    const QColor &diffuse, const QColor &specular)
{
    Material material;
    material.name = name;
    material.ambient = ambient;
    material.diffuse = diffuse;
    material.specular = specular;

    m_materials[name] = material;
}

QString MaterialManager::colorToString(const QColor &color)
{
    return QString("%1 %2 %3 %4")
        .arg(color.redF())
        .arg(color.greenF())
        .arg(color.blueF())
        .arg(color.alphaF());
}

} // namespace Burma
