#include "ui/RenderWidget.h"
#include "utils/Logger.h"
#include <QOpenGLContext>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <cmath>

// Helper functions for legacy OpenGL (will be replaced with modern OpenGL later)
static void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    GLdouble xmin, xmax, ymin, ymax;
    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;
    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

static void gluLookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ,
                     GLdouble centerX, GLdouble centerY, GLdouble centerZ,
                     GLdouble upX, GLdouble upY, GLdouble upZ)
{
    QMatrix4x4 matrix;
    QVector3D eye(eyeX, eyeY, eyeZ);
    QVector3D center(centerX, centerY, centerZ);
    QVector3D up(upX, upY, upZ);
    matrix.lookAt(eye, center, up);
    glMultMatrixf(matrix.constData());
}

#ifdef HAVE_GAZEBO_RENDERING
// TODO: Include Gazebo rendering headers when available
// #include <gz/rendering/RenderEngine.hh>
// #include <gz/rendering/Scene.hh>
// #include <gz/rendering/Camera.hh>
#endif

namespace Burma {

RenderWidget::RenderWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_updateTimer(nullptr)
    , m_gazeboInitialized(false)
    , m_gazeboScene(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Setup update timer for smooth rendering
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &RenderWidget::onUpdateFrame);
    m_updateTimer->start(16); // ~60 FPS
}

RenderWidget::~RenderWidget()
{
    makeCurrent();
    shutdownGazeboRenderer();
    doneCurrent();
}

void RenderWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Set OpenGL parameters
    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Logger::instance().info("OpenGL initialized: " +
                          QString(reinterpret_cast<const char*>(glGetString(GL_VERSION))));

    // Initialize Gazebo renderer
    initializeGazeboRenderer();
}

void RenderWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

#ifdef HAVE_GAZEBO_RENDERING
    // TODO: Update Gazebo camera aspect ratio
#endif
}

void RenderWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_gazeboInitialized) {
        renderScene();
    } else {
        // Render placeholder grid when Gazebo is not available
        renderPlaceholderGrid();
    }
}

void RenderWidget::renderPlaceholderGrid()
{
    // Simple grid rendering for testing without Gazebo
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = static_cast<float>(width()) / static_cast<float>(height());
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Position camera
    float camX = m_camera.distance * cos(m_camera.elevation * M_PI / 180.0f) *
                 cos(m_camera.azimuth * M_PI / 180.0f);
    float camY = m_camera.distance * cos(m_camera.elevation * M_PI / 180.0f) *
                 sin(m_camera.azimuth * M_PI / 180.0f);
    float camZ = m_camera.distance * sin(m_camera.elevation * M_PI / 180.0f);

    gluLookAt(camX, camY, camZ,
              m_camera.targetX, m_camera.targetY, m_camera.targetZ,
              0.0f, 0.0f, 1.0f);

    // Draw grid
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    for (int i = -10; i <= 10; ++i) {
        glVertex3f(i, -10, 0);
        glVertex3f(i, 10, 0);
        glVertex3f(-10, i, 0);
        glVertex3f(10, i, 0);
    }
    glEnd();

    // Draw axes
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f); // X - Red
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);

    glColor3f(0.0f, 1.0f, 0.0f); // Y - Green
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);

    glColor3f(0.0f, 0.0f, 1.0f); // Z - Blue
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);
    glEnd();
}

void RenderWidget::initializeGazeboRenderer()
{
#ifdef HAVE_GAZEBO_RENDERING
    // TODO: Initialize Gazebo rendering engine
    Logger::instance().info("Initializing Gazebo renderer...");
    m_gazeboInitialized = true;
#else
    Logger::instance().warning("Gazebo rendering not available - using placeholder renderer");
    m_gazeboInitialized = false;
#endif
}

void RenderWidget::shutdownGazeboRenderer()
{
#ifdef HAVE_GAZEBO_RENDERING
    // TODO: Cleanup Gazebo renderer
    if (m_gazeboInitialized) {
        Logger::instance().info("Shutting down Gazebo renderer");
    }
#endif
}

void RenderWidget::renderScene()
{
#ifdef HAVE_GAZEBO_RENDERING
    // TODO: Render Gazebo scene
#else
    renderPlaceholderGrid();
#endif
}

void RenderWidget::updateCamera()
{
    update();
}

void RenderWidget::loadWorld(const QString &worldFile)
{
    Logger::instance().info("Loading world: " + worldFile);
    m_currentWorld = worldFile;

#ifdef HAVE_GAZEBO_RENDERING
    // TODO: Load world into Gazebo scene
#endif

    emit worldLoaded(worldFile);
    update();
}

void RenderWidget::clearWorld()
{
    Logger::instance().info("Clearing world");
    m_currentWorld.clear();

#ifdef HAVE_GAZEBO_RENDERING
    // TODO: Clear Gazebo scene
#endif

    update();
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastMousePos = event->pos();

    if (event->button() == Qt::MiddleButton) {
        m_isRotating = true;
    } else if (event->button() == Qt::RightButton) {
        m_isPanning = true;
    }
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint delta = event->pos() - m_lastMousePos;
    m_lastMousePos = event->pos();

    if (m_isRotating) {
        m_camera.azimuth += delta.x() * 0.5f;
        m_camera.elevation += delta.y() * 0.5f;

        // Clamp elevation
        m_camera.elevation = std::max(-89.0f, std::min(89.0f, m_camera.elevation));

        updateCamera();
    } else if (m_isPanning) {
        float panSpeed = m_camera.distance * 0.001f;
        m_camera.targetX -= delta.x() * panSpeed;
        m_camera.targetY += delta.y() * panSpeed;

        updateCamera();
    }
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_isRotating = false;
    } else if (event->button() == Qt::RightButton) {
        m_isPanning = false;
    }
}

void RenderWidget::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() / 120.0f;
    m_camera.distance *= (1.0f - delta * 0.1f);
    m_camera.distance = std::max(1.0f, std::min(100.0f, m_camera.distance));

    updateCamera();
}

void RenderWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Home:
        // Reset camera
        m_camera.distance = 10.0f;
        m_camera.azimuth = 45.0f;
        m_camera.elevation = 30.0f;
        m_camera.targetX = 0.0f;
        m_camera.targetY = 0.0f;
        m_camera.targetZ = 0.0f;
        updateCamera();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void RenderWidget::onUpdateFrame()
{
    if (m_gazeboInitialized) {
        update();
    }
}

} // namespace Burma
