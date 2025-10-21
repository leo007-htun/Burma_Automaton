#include "modules/RvizConverter.h"
#include "utils/Logger.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QImage>
#include <QPainter>

namespace Burma {

RvizConverter::RvizConverter(QObject *parent)
    : QObject(parent)
{
}

void RvizConverter::convertWorld(const QString &worldFile, const QString &outputDir,
                                double resolution, int width, int height)
{
    Logger::instance().info("Converting world to RViz format: " + worldFile);

    // Ensure output directory exists
    QDir dir(outputDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString baseName = QFileInfo(worldFile).completeBaseName();
    QString pgmFile = dir.filePath(baseName + ".pgm");
    QString yamlFile = dir.filePath(baseName + ".yaml");

    emit conversionProgress(10);

    // Generate occupancy grid image
    if (!generateOccupancyGrid(worldFile, pgmFile, resolution, width, height)) {
        emit conversionError("Failed to generate occupancy grid");
        return;
    }

    emit conversionProgress(70);

    // Generate YAML metadata
    if (!generateYamlMetadata(yamlFile, baseName + ".pgm", resolution, width, height)) {
        emit conversionError("Failed to generate YAML metadata");
        return;
    }

    emit conversionProgress(100);
    emit conversionComplete(pgmFile, yamlFile);

    Logger::instance().info("RViz conversion complete");
}

bool RvizConverter::generateOccupancyGrid(const QString &worldFile, const QString &pgmFile,
                                         double resolution, int width, int height)
{
    Logger::instance().info("Generating occupancy grid image...");

    // TODO: Implement actual world parsing and occupancy grid generation
    // For now, create a simple placeholder image

    QImage image(width, height, QImage::Format_Grayscale8);
    image.fill(Qt::white);  // Free space

    QPainter painter(&image);

    // Draw some example obstacles (this would be replaced with actual world parsing)
    painter.setBrush(Qt::black);  // Occupied space

    // Example: draw some rectangles representing obstacles
    painter.drawRect(100, 100, 200, 50);
    painter.drawRect(400, 300, 100, 150);
    painter.drawRect(700, 500, 300, 100);

    painter.end();

    // Save as PGM
    if (!image.save(pgmFile, "PGM")) {
        Logger::instance().error("Failed to save PGM file");
        return false;
    }

    Logger::instance().info("Occupancy grid saved to: " + pgmFile);
    return true;
}

bool RvizConverter::generateYamlMetadata(const QString &yamlFile, const QString &pgmFile,
                                        double resolution, int width, int height)
{
    Logger::instance().info("Generating YAML metadata...");

    // Calculate origin (center of map)
    double originX = -(width * resolution) / 2.0;
    double originY = -(height * resolution) / 2.0;

    QString yaml = generateMapYaml(pgmFile, resolution,
                                  QPointF(originX, originY),
                                  width, height);

    QFile file(yamlFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::instance().error("Failed to open YAML file for writing");
        return false;
    }

    QTextStream out(&file);
    out << yaml;
    file.close();

    Logger::instance().info("YAML metadata saved to: " + yamlFile);
    return true;
}

QString RvizConverter::generateMapYaml(const QString &imageName, double resolution,
                                      const QPointF &origin, int width, int height)
{
    QString yaml;
    QTextStream stream(&yaml);

    stream << "image: " << imageName << "\n";
    stream << "resolution: " << resolution << "\n";
    stream << "origin: [" << origin.x() << ", " << origin.y() << ", 0.0]\n";
    stream << "negate: 0\n";
    stream << "occupied_thresh: 0.65\n";
    stream << "free_thresh: 0.196\n";

    return yaml;
}

} // namespace Burma
