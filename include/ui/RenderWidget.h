#ifndef BURMA_RENDERWIDGET_H
#define BURMA_RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>

namespace Burma {

class RenderWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget *parent = nullptr);
    ~RenderWidget() override;

    void loadWorld(const QString &worldFile);
    void clearWorld();

signals:
    void worldLoaded(const QString &worldFile);
    void selectionChanged(const QString &entityName);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onUpdateFrame();

private:
    void initializeGazeboRenderer();
    void shutdownGazeboRenderer();
    void renderScene();
    void renderPlaceholderGrid();
    void updateCamera();

    // Camera control
    struct Camera {
        float distance = 10.0f;
        float azimuth = 45.0f;
        float elevation = 30.0f;
        float targetX = 0.0f;
        float targetY = 0.0f;
        float targetZ = 0.0f;
    } m_camera;

    // Mouse interaction
    QPoint m_lastMousePos;
    bool m_isRotating = false;
    bool m_isPanning = false;

    // Update timer
    QTimer *m_updateTimer;

    // Gazebo integration
    bool m_gazeboInitialized = false;
    void *m_gazeboScene = nullptr; // Will be gz::rendering::ScenePtr when Gazebo is integrated

    // Current world
    QString m_currentWorld;
};

} // namespace Burma

#endif // BURMA_RENDERWIDGET_H
