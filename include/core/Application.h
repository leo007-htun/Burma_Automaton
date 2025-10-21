#ifndef BURMA_APPLICATION_H
#define BURMA_APPLICATION_H

#include <QObject>
#include <QString>
#include <QSettings>

namespace Burma {

class BitNetClient;
class FuelFetcher;
class SDFBuilder;
class RvizConverter;
class MaterialManager;

class Application : public QObject
{
    Q_OBJECT

public:
    static Application& instance();

    void initialize();
    void shutdown();

    // Getters for core modules
    BitNetClient* bitNetClient() const { return m_bitNetClient; }
    FuelFetcher* fuelFetcher() const { return m_fuelFetcher; }
    SDFBuilder* sdfBuilder() const { return m_sdfBuilder; }
    RvizConverter* rvizConverter() const { return m_rvizConverter; }
    MaterialManager* materialManager() const { return m_materialManager; }

    // Settings
    QSettings& settings() { return m_settings; }

signals:
    void initialized();
    void shutdownRequested();

private:
    Application();
    ~Application() override;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void initializeModules();
    void shutdownModules();

    // Core modules
    BitNetClient *m_bitNetClient;
    FuelFetcher *m_fuelFetcher;
    SDFBuilder *m_sdfBuilder;
    RvizConverter *m_rvizConverter;
    MaterialManager *m_materialManager;

    // Settings
    QSettings m_settings;

    bool m_initialized;
};

} // namespace Burma

#endif // BURMA_APPLICATION_H
