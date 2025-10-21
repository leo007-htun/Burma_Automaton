#include "modules/SDFBuilder.h"
#include "utils/Logger.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>

namespace Burma {

SDFBuilder::SDFBuilder(QObject *parent)
    : QObject(parent)
{
}

QString SDFBuilder::buildWorldSDF(const QJsonObject &worldPlan)
{
    Logger::instance().info("Building SDF world from plan...");

    if (worldPlan.isEmpty()) {
        Logger::instance().error("Empty world plan provided");
        emit buildError("Empty world plan");
        return QString();
    }

    QString worldName = worldPlan.value("world_name").toString("generated_world");
    QJsonObject physics = worldPlan.value("physics").toObject();
    QJsonArray lights = worldPlan.value("lighting").toArray();
    QJsonArray models = worldPlan.value("models").toArray();

    QString sdf;
    QTextStream stream(&sdf);

    // XML header
    stream << "<?xml version=\"1.0\" ?>\n";

    // World header
    stream << generateWorldHeader(worldName);

    // Physics
    stream << generatePhysicsSection(physics);

    // Lighting
    stream << generateLightingSection(lights);

    // Models
    stream << generateModelsSection(models);

    // Close world tag
    stream << "</world>\n";
    stream << "</sdf>\n";

    Logger::instance().info("SDF world generated successfully");
    return sdf;
}

bool SDFBuilder::saveToFile(const QString &sdfContent, const QString &filePath)
{
    Logger::instance().info("Saving SDF to: " + filePath);

    // Ensure directory exists
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString error = "Failed to open file for writing: " + file.errorString();
        Logger::instance().error(error);
        emit buildError(error);
        return false;
    }

    QTextStream out(&file);
    out << sdfContent;
    file.close();

    Logger::instance().info("SDF saved successfully");
    emit buildComplete(filePath);
    return true;
}

QString SDFBuilder::buildModelSDF(const QJsonObject &modelData)
{
    return generateModelElement(modelData);
}

QString SDFBuilder::generateWorldHeader(const QString &worldName)
{
    return QString(
        "<sdf version=\"1.9\">\n"
        "  <world name=\"%1\">\n"
    ).arg(escapeXML(worldName));
}

QString SDFBuilder::generatePhysicsSection(const QJsonObject &physicsData)
{
    QString gravity = physicsData.value("gravity").toString("0 0 -9.81");
    double maxStepSize = physicsData.value("max_step_size").toDouble(0.001);
    double realTimeFactor = physicsData.value("real_time_factor").toDouble(1.0);

    return QString(
        "    <physics type=\"ode\">\n"
        "      <max_step_size>%1</max_step_size>\n"
        "      <real_time_factor>%2</real_time_factor>\n"
        "      <gravity>%3</gravity>\n"
        "    </physics>\n"
    ).arg(maxStepSize).arg(realTimeFactor).arg(gravity);
}

QString SDFBuilder::generateLightingSection(const QJsonArray &lights)
{
    QString result;

    if (lights.isEmpty()) {
        // Add default sun light
        result += "    <light type=\"directional\" name=\"sun\">\n"
                 "      <pose>0 0 10 0 0 0</pose>\n"
                 "      <diffuse>1 1 1 1</diffuse>\n"
                 "      <specular>0.5 0.5 0.5 1</specular>\n"
                 "      <direction>-0.5 0.1 -0.9</direction>\n"
                 "    </light>\n";
    } else {
        for (const QJsonValue &lightVal : lights) {
            QJsonObject light = lightVal.toObject();
            QString name = light.value("name").toString("light");
            QString type = light.value("type").toString("directional");

            result += QString("    <light type=\"%1\" name=\"%2\">\n").arg(type, name);

            QJsonObject pos = light.value("position").toObject();
            result += QString("      <pose>%1 %2 %3 0 0 0</pose>\n")
                .arg(pos.value("x").toDouble(0))
                .arg(pos.value("y").toDouble(0))
                .arg(pos.value("z").toDouble(10));

            QString diffuse = light.value("diffuse").toString("1 1 1 1");
            result += QString("      <diffuse>%1</diffuse>\n").arg(diffuse);

            result += "    </light>\n";
        }
    }

    return result;
}

QString SDFBuilder::generateModelsSection(const QJsonArray &models)
{
    QString result;

    for (const QJsonValue &modelVal : models) {
        QJsonObject model = modelVal.toObject();
        result += generateModelElement(model);
    }

    return result;
}

QString SDFBuilder::generateModelElement(const QJsonObject &model)
{
    QString name = model.value("name").toString("unnamed_model");
    QString type = model.value("type").toString("box");
    bool isStatic = model.value("static").toBool(false);

    QString result = QString("    <model name=\"%1\">\n").arg(escapeXML(name));

    if (isStatic) {
        result += "      <static>true</static>\n";
    }

    // Pose
    QJsonObject position = model.value("position").toObject();
    QJsonObject rotation = model.value("rotation").toObject();
    result += generatePoseElement(position, rotation);

    // Link
    result += "      <link name=\"link\">\n";

    // Visual
    result += generateVisualElement(model);

    // Collision
    result += generateCollisionElement(model);

    result += "      </link>\n";
    result += "    </model>\n";

    return result;
}

QString SDFBuilder::generatePoseElement(const QJsonObject &position, const QJsonObject &rotation)
{
    double x = position.value("x").toDouble(0);
    double y = position.value("y").toDouble(0);
    double z = position.value("z").toDouble(0);

    double roll = rotation.value("roll").toDouble(0);
    double pitch = rotation.value("pitch").toDouble(0);
    double yaw = rotation.value("yaw").toDouble(0);

    return QString("      <pose>%1 %2 %3 %4 %5 %6</pose>\n")
        .arg(x).arg(y).arg(z).arg(roll).arg(pitch).arg(yaw);
}

QString SDFBuilder::generateScaleElement(const QJsonObject &scale)
{
    double x = scale.value("x").toDouble(1);
    double y = scale.value("y").toDouble(1);
    double z = scale.value("z").toDouble(1);

    return QString("          <scale>%1 %2 %3</scale>\n").arg(x).arg(y).arg(z);
}

QString SDFBuilder::generateVisualElement(const QJsonObject &model)
{
    QString type = model.value("type").toString("box");
    QJsonObject scale = model.value("scale").toObject();
    QString color = model.value("color").toString("#FFFFFF");

    QString result = "        <visual name=\"visual\">\n";
    result += "          <geometry>\n";

    if (type == "box") {
        double sx = scale.value("x").toDouble(1);
        double sy = scale.value("y").toDouble(1);
        double sz = scale.value("z").toDouble(1);
        result += QString("            <box><size>%1 %2 %3</size></box>\n").arg(sx).arg(sy).arg(sz);
    } else if (type == "sphere") {
        double radius = scale.value("x").toDouble(0.5);
        result += QString("            <sphere><radius>%1</radius></sphere>\n").arg(radius);
    } else if (type == "cylinder") {
        double radius = scale.value("x").toDouble(0.5);
        double length = scale.value("z").toDouble(1.0);
        result += QString("            <cylinder><radius>%1</radius><length>%2</length></cylinder>\n")
            .arg(radius).arg(length);
    }

    result += "          </geometry>\n";

    // Material/Color
    if (!color.isEmpty()) {
        result += "          <material>\n";
        result += QString("            <ambient>%1</ambient>\n").arg(color);
        result += QString("            <diffuse>%1</diffuse>\n").arg(color);
        result += "          </material>\n";
    }

    result += "        </visual>\n";

    return result;
}

QString SDFBuilder::generateCollisionElement(const QJsonObject &model)
{
    QString type = model.value("type").toString("box");
    QJsonObject scale = model.value("scale").toObject();

    QString result = "        <collision name=\"collision\">\n";
    result += "          <geometry>\n";

    if (type == "box") {
        double sx = scale.value("x").toDouble(1);
        double sy = scale.value("y").toDouble(1);
        double sz = scale.value("z").toDouble(1);
        result += QString("            <box><size>%1 %2 %3</size></box>\n").arg(sx).arg(sy).arg(sz);
    } else if (type == "sphere") {
        double radius = scale.value("x").toDouble(0.5);
        result += QString("            <sphere><radius>%1</radius></sphere>\n").arg(radius);
    } else if (type == "cylinder") {
        double radius = scale.value("x").toDouble(0.5);
        double length = scale.value("z").toDouble(1.0);
        result += QString("            <cylinder><radius>%1</radius><length>%2</length></cylinder>\n")
            .arg(radius).arg(length);
    }

    result += "          </geometry>\n";
    result += "        </collision>\n";

    return result;
}

QString SDFBuilder::escapeXML(const QString &text)
{
    QString escaped = text;
    escaped.replace("&", "&amp;");
    escaped.replace("<", "&lt;");
    escaped.replace(">", "&gt;");
    escaped.replace("\"", "&quot;");
    escaped.replace("'", "&apos;");
    return escaped;
}

} // namespace Burma
