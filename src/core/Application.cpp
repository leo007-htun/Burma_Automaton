#include "core/Application.h"
#include "modules/BitNetClient.h"
#include "modules/FuelFetcher.h"
#include "modules/SDFBuilder.h"
#include "modules/RvizConverter.h"
#include "modules/MaterialManager.h"
#include "utils/Logger.h"

namespace Burma {

Application::Application()
    : m_bitNetClient(nullptr)
    , m_fuelFetcher(nullptr)
    , m_sdfBuilder(nullptr)
    , m_rvizConverter(nullptr)
    , m_materialManager(nullptr)
    , m_settings("BurmaRobotics", "BurmaAutomaton")
    , m_initialized(false)
{
}

Application::~Application()
{
    shutdown();
}

Application& Application::instance()
{
    static Application instance;
    return instance;
}

void Application::initialize()
{
    if (m_initialized) {
        return;
    }

    Logger::instance().info("Initializing Burma Automaton application...");

    // Initialize core modules
    initializeModules();

    m_initialized = true;
    emit initialized();

    Logger::instance().info("Application initialization complete");
}

void Application::shutdown()
{
    if (!m_initialized) {
        return;
    }

    Logger::instance().info("Shutting down application...");

    emit shutdownRequested();

    // Shutdown modules
    shutdownModules();

    m_initialized = false;

    Logger::instance().info("Application shutdown complete");
}

void Application::initializeModules()
{
    // Initialize BitNet LLM client
    m_bitNetClient = new BitNetClient(this);
    Logger::instance().info("BitNet client initialized");

    // Initialize Gazebo Fuel fetcher
    m_fuelFetcher = new FuelFetcher(this);
    Logger::instance().info("Fuel fetcher initialized");

    // Initialize SDF builder
    m_sdfBuilder = new SDFBuilder(this);
    Logger::instance().info("SDF builder initialized");

    // Initialize RViz converter
    m_rvizConverter = new RvizConverter(this);
    Logger::instance().info("RViz converter initialized");

    // Initialize material manager
    m_materialManager = new MaterialManager(this);
    Logger::instance().info("Material manager initialized");
}

void Application::shutdownModules()
{
    // Delete modules in reverse order
    delete m_materialManager;
    m_materialManager = nullptr;

    delete m_rvizConverter;
    m_rvizConverter = nullptr;

    delete m_sdfBuilder;
    m_sdfBuilder = nullptr;

    delete m_fuelFetcher;
    m_fuelFetcher = nullptr;

    delete m_bitNetClient;
    m_bitNetClient = nullptr;
}

} // namespace Burma
