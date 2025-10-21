#include <QApplication>
#include <QStyleFactory>
#include "ui/MainWindow.h"
#include "core/Application.h"
#include "utils/Logger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon("/usr/share/icons/hicolor/256x256/apps/burma-automaton.png"));

    // Set application metadata
    QApplication::setApplicationName("Burma Automaton");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Burma Robotics");
    QApplication::setOrganizationDomain("burma-automaton.org");

    // Set modern style
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // Initialize logger
    Burma::Logger::instance().initialize();
    Burma::Logger::instance().info("Burma Automaton starting...");

    // Initialize core application
    Burma::Application::instance().initialize();

    // Create and show main window
    Burma::MainWindow mainWindow;
    mainWindow.show();

    Burma::Logger::instance().info("Application initialized successfully");

    return app.exec();
}
