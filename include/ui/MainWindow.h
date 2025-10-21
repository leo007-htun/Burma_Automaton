#ifndef BURMA_MAINWINDOW_H
#define BURMA_MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QJsonObject>

namespace Burma {

class PromptPanel;
class RenderWidget;
class PropertyEditor;
class EventLog;
class AssetBrowser;
class ExportPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onNewWorld();
    void onOpenWorld();
    void onSaveWorld();
    void onSaveWorldAs();
    void onExportRViz();
    void onSettings();
    void onAbout();
    void onProcessPrompt(const QString &prompt);
    void onWorldGenerated(const QString &sdfPath);

    // BitNet slots
    void onWorldPlanGenerated(const QJsonObject &worldPlan);
    void onBitNetError(const QString &error);
    void onBitNetProcessingStarted();
    void onBitNetProcessingFinished();

private:
    void setupUi();
    void createMenus();
    void createToolbars();
    void createDockWidgets();
    void setupConnections();

    // Central widget
    RenderWidget *m_renderWidget;

    // Dock widgets
    QDockWidget *m_promptDock;
    QDockWidget *m_propertyDock;
    QDockWidget *m_eventLogDock;
    QDockWidget *m_assetBrowserDock;
    QDockWidget *m_exportDock;

    // Panels
    PromptPanel *m_promptPanel;
    PropertyEditor *m_propertyEditor;
    EventLog *m_eventLog;
    AssetBrowser *m_assetBrowser;
    ExportPanel *m_exportPanel;

    // Menus
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_toolsMenu;
    QMenu *m_helpMenu;

    // Toolbars
    QToolBar *m_mainToolBar;

    // Current world file
    QString m_currentWorldFile;
};

} // namespace Burma

#endif // BURMA_MAINWINDOW_H
