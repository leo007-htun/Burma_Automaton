#ifndef BURMA_RVIZCONVERTER_H
#define BURMA_RVIZCONVERTER_H

#include <QObject>
#include <QString>

namespace Burma {

/**
 * @brief Converts Gazebo worlds to RViz-compatible map formats
 *
 * Exports .pgm and .yaml files for use in ROS navigation
 * Based on: https://github.com/leo007-htun/Gazebo-map-to-Rviz
 */
class RvizConverter : public QObject
{
    Q_OBJECT

public:
    explicit RvizConverter(QObject *parent = nullptr);
    ~RvizConverter() override = default;

    // Convert world to RViz map
    void convertWorld(const QString &worldFile, const QString &outputDir,
                     double resolution = 0.05, int width = 2000, int height = 2000);

signals:
    void conversionProgress(int percentage);
    void conversionComplete(const QString &pgmFile, const QString &yamlFile);
    void conversionError(const QString &error);

private:
    bool generateOccupancyGrid(const QString &worldFile, const QString &pgmFile,
                              double resolution, int width, int height);
    bool generateYamlMetadata(const QString &yamlFile, const QString &pgmFile,
                             double resolution, int width, int height);

    QString generateMapYaml(const QString &imageName, double resolution,
                           const QPointF &origin, int width, int height);
};

} // namespace Burma

#endif // BURMA_RVIZCONVERTER_H
