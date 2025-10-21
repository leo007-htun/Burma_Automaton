#include "ui/MainWindow.h"
#include "ui/PromptPanel.h"
#include "ui/RenderWidget.h"
#include "ui/PropertyEditor.h"
#include "ui/EventLog.h"
#include "ui/AssetBrowser.h"
#include "ui/ExportPanel.h"
#include "core/Application.h"
#include "modules/BitNetClient.h"
#include "modules/SDFBuilder.h"
#include "utils/Logger.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>
#include <QAction>
#include <QIcon>
#include <QDir>
#include <QDateTime>

namespace Burma {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_renderWidget(nullptr)
    , m_promptDock(nullptr)
    , m_propertyDock(nullptr)
    , m_eventLogDock(nullptr)
    , m_assetBrowserDock(nullptr)
    , m_exportDock(nullptr)
    , m_promptPanel(nullptr)
    , m_propertyEditor(nullptr)
    , m_eventLog(nullptr)
    , m_assetBrowser(nullptr)
    , m_exportPanel(nullptr)
{
    setupUi();
    createMenus();
    createToolbars();
    createDockWidgets();
    setupConnections();

    setWindowTitle("Burma Automaton - Gazebo World Builder");
    resize(1600, 1000);

    // Restore window geometry
    QSettings settings;
    restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
    restoreState(settings.value("mainWindow/state").toByteArray());

    statusBar()->showMessage("Ready", 3000);
}

MainWindow::~MainWindow()
{
    // Save window geometry
    QSettings settings;
    settings.setValue("mainWindow/geometry", saveGeometry());
    settings.setValue("mainWindow/state", saveState());
}

void MainWindow::setupUi()
{
    // Create central render widget
    m_renderWidget = new RenderWidget(this);
    setCentralWidget(m_renderWidget);
}

void MainWindow::createMenus()
{
    // File Menu
    m_fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *newAction = m_fileMenu->addAction(tr("&New World"));
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewWorld);

    QAction *openAction = m_fileMenu->addAction(tr("&Open World..."));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenWorld);

    m_fileMenu->addSeparator();

    QAction *saveAction = m_fileMenu->addAction(tr("&Save World"));
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveWorld);

    QAction *saveAsAction = m_fileMenu->addAction(tr("Save World &As..."));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::onSaveWorldAs);

    m_fileMenu->addSeparator();

    QAction *exportAction = m_fileMenu->addAction(tr("&Export to RViz..."));
    exportAction->setShortcut(QKeySequence(tr("Ctrl+E")));
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportRViz);

    m_fileMenu->addSeparator();

    QAction *quitAction = m_fileMenu->addAction(tr("&Quit"));
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    // Edit Menu
    m_editMenu = menuBar()->addMenu(tr("&Edit"));

    QAction *settingsAction = m_editMenu->addAction(tr("&Settings..."));
    settingsAction->setShortcut(QKeySequence::Preferences);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettings);

    // View Menu
    m_viewMenu = menuBar()->addMenu(tr("&View"));
    // Dock widget toggles will be added in createDockWidgets()

    // Tools Menu
    m_toolsMenu = menuBar()->addMenu(tr("&Tools"));

    // Help Menu
    m_helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAction = m_helpMenu->addAction(tr("&About Burma Automaton"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);

    QAction *aboutQtAction = m_helpMenu->addAction(tr("About &Qt"));
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::createToolbars()
{
    m_mainToolBar = addToolBar(tr("Main Toolbar"));
    m_mainToolBar->setObjectName("MainToolbar");

    // Add common actions
    m_mainToolBar->addAction(tr("New"), this, &MainWindow::onNewWorld);
    m_mainToolBar->addAction(tr("Open"), this, &MainWindow::onOpenWorld);
    m_mainToolBar->addAction(tr("Save"), this, &MainWindow::onSaveWorld);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(tr("Export"), this, &MainWindow::onExportRViz);
}

void MainWindow::createDockWidgets()
{
    // Prompt Panel (Left side, top)
    m_promptPanel = new PromptPanel(this);
    m_promptDock = new QDockWidget(tr("Natural Language Prompt"), this);
    m_promptDock->setObjectName("PromptDock");
    m_promptDock->setWidget(m_promptPanel);
    addDockWidget(Qt::LeftDockWidgetArea, m_promptDock);
    m_viewMenu->addAction(m_promptDock->toggleViewAction());

    // Property Editor (Right side)
    m_propertyEditor = new PropertyEditor(this);
    m_propertyDock = new QDockWidget(tr("Properties"), this);
    m_propertyDock->setObjectName("PropertyDock");
    m_propertyDock->setWidget(m_propertyEditor);
    addDockWidget(Qt::RightDockWidgetArea, m_propertyDock);
    m_viewMenu->addAction(m_propertyDock->toggleViewAction());

    // Asset Browser (Left side, bottom)
    m_assetBrowser = new AssetBrowser(this);
    m_assetBrowserDock = new QDockWidget(tr("Asset Browser"), this);
    m_assetBrowserDock->setObjectName("AssetBrowserDock");
    m_assetBrowserDock->setWidget(m_assetBrowser);
    addDockWidget(Qt::LeftDockWidgetArea, m_assetBrowserDock);
    m_viewMenu->addAction(m_assetBrowserDock->toggleViewAction());

    // Event Log (Bottom)
    m_eventLog = new EventLog(this);
    m_eventLogDock = new QDockWidget(tr("Event Log"), this);
    m_eventLogDock->setObjectName("EventLogDock");
    m_eventLogDock->setWidget(m_eventLog);
    addDockWidget(Qt::BottomDockWidgetArea, m_eventLogDock);
    m_viewMenu->addAction(m_eventLogDock->toggleViewAction());

    // Export Panel (Right side, below properties)
    m_exportPanel = new ExportPanel(this);
    m_exportDock = new QDockWidget(tr("Export"), this);
    m_exportDock->setObjectName("ExportDock");
    m_exportDock->setWidget(m_exportPanel);
    addDockWidget(Qt::RightDockWidgetArea, m_exportDock);
    m_viewMenu->addAction(m_exportDock->toggleViewAction());

    // Split docks vertically on left side
    splitDockWidget(m_promptDock, m_assetBrowserDock, Qt::Vertical);

    // Split docks vertically on right side
    splitDockWidget(m_propertyDock, m_exportDock, Qt::Vertical);
}

void MainWindow::setupConnections()
{
    // Connect prompt panel to processing
    connect(m_promptPanel, &PromptPanel::promptSubmitted,
            this, &MainWindow::onProcessPrompt);

    // Connect event log to logger
    connect(&Logger::instance(), &Logger::messageLogged,
            m_eventLog, &EventLog::appendMessage);

    // Connect BitNetClient signals
    BitNetClient *bitNetClient = Application::instance().bitNetClient();
    if (bitNetClient) {
        connect(bitNetClient, &BitNetClient::worldPlanGenerated,
                this, &MainWindow::onWorldPlanGenerated);
        connect(bitNetClient, &BitNetClient::errorOccurred,
                this, &MainWindow::onBitNetError);
        connect(bitNetClient, &BitNetClient::processingStarted,
                this, &MainWindow::onBitNetProcessingStarted);
        connect(bitNetClient, &BitNetClient::processingFinished,
                this, &MainWindow::onBitNetProcessingFinished);
    }
}

void MainWindow::onNewWorld()
{
    Logger::instance().info("Creating new world...");
    m_currentWorldFile.clear();
    m_renderWidget->clearWorld();
    statusBar()->showMessage("New world created", 3000);
}

void MainWindow::onOpenWorld()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open World File"),
        QString(),
        tr("SDF Files (*.sdf *.world);;All Files (*)")
    );

    if (!fileName.isEmpty()) {
        Logger::instance().info("Opening world: " + fileName);
        m_currentWorldFile = fileName;
        m_renderWidget->loadWorld(fileName);
        statusBar()->showMessage("World loaded: " + fileName, 3000);
    }
}

void MainWindow::onSaveWorld()
{
    if (m_currentWorldFile.isEmpty()) {
        onSaveWorldAs();
    } else {
        Logger::instance().info("Saving world: " + m_currentWorldFile);
        // TODO: Implement actual save functionality
        statusBar()->showMessage("World saved", 3000);
    }
}

void MainWindow::onSaveWorldAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save World File"),
        QString(),
        tr("SDF Files (*.sdf);;World Files (*.world);;All Files (*)")
    );

    if (!fileName.isEmpty()) {
        m_currentWorldFile = fileName;
        Logger::instance().info("Saving world as: " + fileName);
        // TODO: Implement actual save functionality
        statusBar()->showMessage("World saved as: " + fileName, 3000);
    }
}

void MainWindow::onExportRViz()
{
    Logger::instance().info("Exporting to RViz format...");
    m_exportPanel->showExportDialog();
}

void MainWindow::onSettings()
{
    Logger::instance().info("Opening settings dialog...");
    // TODO: Implement settings dialog
    QMessageBox::information(this, tr("Settings"),
                           tr("Settings dialog not yet implemented"));
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("About Burma Automaton"),
        tr("<h2>Burma Automaton v1.0.0</h2>"
           "<p>Natural language-driven Gazebo world builder</p>"
           "<p>Built with Qt6 and Gazebo/Ignition</p>"
           "<p>Powered by BitNet LLM for intelligent world generation</p>"
           "<p>Copyright © 2025 Burma Robotics</p>"));
}

void MainWindow::onProcessPrompt(const QString &prompt)
{
    Logger::instance().info("Processing prompt: " + prompt);
    statusBar()->showMessage("Processing prompt with BitNet...");

    // Send prompt to BitNet client for processing
    BitNetClient *bitNetClient = Application::instance().bitNetClient();
    if (bitNetClient) {
        bitNetClient->processPrompt(prompt);
    } else {
        Logger::instance().error("BitNetClient not available");
        statusBar()->showMessage("Error: BitNet client not initialized", 5000);
    }
}

void MainWindow::onWorldGenerated(const QString &sdfPath)
{
    Logger::instance().info("Loading generated world: " + sdfPath);
    m_renderWidget->loadWorld(sdfPath);
    statusBar()->showMessage("World generated and loaded", 3000);
}

void MainWindow::onWorldPlanGenerated(const QJsonObject &worldPlan)
{
    Logger::instance().info("World plan received from BitNet");

    // Build SDF from world plan
    SDFBuilder *sdfBuilder = Application::instance().sdfBuilder();
    if (!sdfBuilder) {
        Logger::instance().error("SDFBuilder not available");
        return;
    }

    QString sdfContent = sdfBuilder->buildWorldSDF(worldPlan);
    if (sdfContent.isEmpty()) {
        Logger::instance().error("Failed to build SDF from world plan");
        statusBar()->showMessage("Error: Failed to generate world", 5000);
        return;
    }

    // Save SDF to temporary file
    QString tempPath = QDir::tempPath() + "/burma_world_" +
                      QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".sdf";

    if (sdfBuilder->saveToFile(sdfContent, tempPath)) {
        onWorldGenerated(tempPath);
    } else {
        Logger::instance().error("Failed to save SDF file");
        statusBar()->showMessage("Error: Failed to save world file", 5000);
    }
}

void MainWindow::onBitNetError(const QString &error)
{
    Logger::instance().error("BitNet error: " + error);
    statusBar()->showMessage("Error: " + error, 5000);

    QMessageBox::warning(this, tr("BitNet Error"),
                        tr("Failed to process prompt:\n%1\n\nMake sure the BitNet server is running.").arg(error));
}

void MainWindow::onBitNetProcessingStarted()
{
    Logger::instance().info("BitNet processing started");
    statusBar()->showMessage("Processing with BitNet...");
}

void MainWindow::onBitNetProcessingFinished()
{
    Logger::instance().info("BitNet processing finished");
    statusBar()->showMessage("Processing complete", 3000);
}

} // namespace Burma
