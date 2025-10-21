#include "modules/BitNetClient.h"
#include "utils/Logger.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

namespace Burma {

BitNetClient::BitNetClient(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_isReady(false)
{
    // Default BitNet.cpp paths
    QString homeDir = QDir::homePath();
    m_bitnetCliPath = homeDir + "/BitNet/build/bin/llama-cli";
    // Use official BitNet 2B GGUF model
    m_modelPath = homeDir + "/BitNet/models/BitNet-b1.58-2B-4T/ggml-model-i2_s.gguf";

    // Check for environment variable overrides
    QByteArray envBitnetPath = qgetenv("BURMA_BITNET_CLI");
    if (!envBitnetPath.isEmpty()) {
        m_bitnetCliPath = QString::fromUtf8(envBitnetPath);
    }

    QByteArray envModelPath = qgetenv("BURMA_BITNET_MODEL");
    if (!envModelPath.isEmpty()) {
        m_modelPath = QString::fromUtf8(envModelPath);
    }

    // Check if BitNet.cpp is available
    if (QFile::exists(m_bitnetCliPath) && QFile::exists(m_modelPath)) {
        m_isReady = true;
    }

    // Connect process signals
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &BitNetClient::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred,
            this, &BitNetClient::onProcessError);

    Logger::instance().info("================================");
    Logger::instance().info("BitNet.cpp Client Initialized");
    Logger::instance().info("CLI Path: " + m_bitnetCliPath);
    Logger::instance().info("Model Path: " + m_modelPath);
    Logger::instance().info("Ready: " + QString(m_isReady ? "Yes" : "No"));
    Logger::instance().info("================================");
}

BitNetClient::~BitNetClient()
{
    if (m_process->state() == QProcess::Running) {
        m_process->kill();
        m_process->waitForFinished();
    }
}

void BitNetClient::setBitNetPath(const QString &path)
{
    m_bitnetCliPath = path;
    m_isReady = QFile::exists(m_bitnetCliPath) && QFile::exists(m_modelPath);
    Logger::instance().info("BitNet CLI path set to: " + path);
}

void BitNetClient::setModelPath(const QString &path)
{
    m_modelPath = path;
    m_isReady = QFile::exists(m_bitnetCliPath) && QFile::exists(m_modelPath);
    Logger::instance().info("BitNet model path set to: " + path);
}

void BitNetClient::processPrompt(const QString &prompt, const QStringList &context)
{
    if (!m_isReady) {
        emit errorOccurred("BitNet.cpp not ready. Please run ./build.sh --setup-bitnet");
        return;
    }

    if (m_process->state() == QProcess::Running) {
        emit errorOccurred("BitNet.cpp is already processing a request");
        return;
    }

    Logger::instance().info("Processing prompt with BitNet.cpp: " + prompt);
    emit processingStarted();

    // Create full prompt with system instructions
    QString fullPrompt = createWorldPlanPrompt(prompt, context);

    // Run inference
    runInference(fullPrompt);
}

QString BitNetClient::createWorldPlanPrompt(const QString &prompt, const QStringList &context)
{
    // Ultra-simple prompt optimized for 2B model - ONE object only
    QString systemPrompt =
        prompt + "\n\n"
        "JSON:\n"
        "{\"world_name\":\"world\",\"models\":[{\"name\":\"box\",\"type\":\"box\","
        "\"position\":{\"x\":0,\"y\":0,\"z\":0},\"rotation\":{\"roll\":0,\"pitch\":0,\"yaw\":0},"
        "\"scale\":{\"x\":1,\"y\":1,\"z\":1},\"color\":\"#FF0000\",\"static\":false}],"
        "\"lighting\":[{\"name\":\"sun\",\"type\":\"directional\",\"position\":{\"x\":0,\"y\":0,\"z\":10}}],"
        "\"physics\":{\"gravity\":\"0 0 -9.81\",\"max_step_size\":0.001}}";

    return systemPrompt;
}

void BitNetClient::runInference(const QString &prompt)
{
    // Prepare arguments for llama-cli
    QStringList arguments;
    arguments << "-m" << m_modelPath;
    arguments << "-p" << prompt;
    arguments << "-n" << "200";        // Minimal tokens - just copy the template
    arguments << "-t" << "4";          // Threads
    arguments << "-ngl" << "0";        // No GPU layers (CPU only)
    arguments << "-c" << "1024";       // Small context for simple task
    arguments << "--temp" << "0.3";    // Lower temperature for more focused output
    arguments << "-b" << "1";          // Batch size
    arguments << "--top-p" << "0.9";   // Nucleus sampling
    arguments << "--repeat-penalty" << "1.1"; // Prevent repetition
    arguments << "--no-display-prompt"; // Don't echo the prompt

    Logger::instance().debug("Running BitNet.cpp: " + m_bitnetCliPath + " " + arguments.join(" "));

    // Start the process
    m_process->start(m_bitnetCliPath, arguments);
}

void BitNetClient::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString error = QString("BitNet.cpp process failed with exit code %1: %2")
                            .arg(exitCode)
                            .arg(QString::fromUtf8(m_process->readAllStandardError()));
        Logger::instance().error(error);

        // Use fallback world on error
        Logger::instance().info("Using fallback: creating simple default world");
        emit worldPlanGenerated(createFallbackWorld());
        emit processingFinished();
        return;
    }

    // Read output
    QString output = QString::fromUtf8(m_process->readAllStandardOutput());
    Logger::instance().debug("BitNet.cpp output: " + output);

    // Parse response
    QJsonObject worldPlan = parseResponse(output);

    if (!worldPlan.isEmpty()) {
        Logger::instance().info("World plan generated successfully");
        emit worldPlanGenerated(worldPlan);
    } else {
        // BitNet failed to generate valid JSON - use fallback
        Logger::instance().warning("BitNet.cpp output invalid, using fallback world");
        emit worldPlanGenerated(createFallbackWorld());
    }

    emit processingFinished();
}

QJsonObject BitNetClient::createFallbackWorld()
{
    // Create a simple default world when BitNet fails
    QJsonObject world;
    world["world_name"] = "default";

    // Create a simple red box
    QJsonObject model;
    model["name"] = "box";
    model["type"] = "box";

    QJsonObject position;
    position["x"] = 0;
    position["y"] = 0;
    position["z"] = 0.5;
    model["position"] = position;

    QJsonObject rotation;
    rotation["roll"] = 0;
    rotation["pitch"] = 0;
    rotation["yaw"] = 0;
    model["rotation"] = rotation;

    QJsonObject scale;
    scale["x"] = 1;
    scale["y"] = 1;
    scale["z"] = 1;
    model["scale"] = scale;

    model["color"] = "#FF0000";
    model["static"] = false;

    QJsonArray models;
    models.append(model);
    world["models"] = models;

    // Add lighting
    QJsonObject light;
    light["name"] = "sun";
    light["type"] = "directional";
    QJsonObject lightPos;
    lightPos["x"] = 0;
    lightPos["y"] = 0;
    lightPos["z"] = 10;
    light["position"] = lightPos;

    QJsonArray lighting;
    lighting.append(light);
    world["lighting"] = lighting;

    // Add physics
    QJsonObject physics;
    physics["gravity"] = "0 0 -9.81";
    physics["max_step_size"] = 0.001;
    world["physics"] = physics;

    return world;
}

void BitNetClient::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "BitNet.cpp failed to start. Check if " + m_bitnetCliPath + " exists";
            break;
        case QProcess::Crashed:
            errorMsg = "BitNet.cpp crashed during execution";
            break;
        case QProcess::Timedout:
            errorMsg = "BitNet.cpp process timed out";
            break;
        default:
            errorMsg = "BitNet.cpp process error: " + QString::number(error);
            break;
    }

    Logger::instance().error(errorMsg);
    emit errorOccurred(errorMsg);
    emit processingFinished();
}

QJsonObject BitNetClient::parseResponse(const QString &output)
{
    // Try to find JSON in the output
    // BitNet.cpp output may contain debug info, so extract JSON portion

    int jsonStart = output.indexOf('{');
    int jsonEnd = output.lastIndexOf('}');

    if (jsonStart == -1 || jsonEnd == -1 || jsonStart >= jsonEnd) {
        Logger::instance().warning("No JSON found in output");
        return QJsonObject();
    }

    QString jsonStr = output.mid(jsonStart, jsonEnd - jsonStart + 1);

    // Try to fix truncated JSON by closing arrays/objects
    if (jsonStr.count('{') > jsonStr.count('}')) {
        // Add missing closing braces
        int missing = jsonStr.count('{') - jsonStr.count('}');
        for (int i = 0; i < missing; i++) {
            jsonStr.append("}");
        }
    }
    if (jsonStr.count('[') > jsonStr.count(']')) {
        // Add missing closing brackets
        int missing = jsonStr.count('[') - jsonStr.count(']');
        for (int i = 0; i < missing; i++) {
            jsonStr.append("]");
        }
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        Logger::instance().error("JSON parse error: " + parseError.errorString());
        Logger::instance().debug("Attempted JSON: " + jsonStr);
        return QJsonObject();
    }

    if (!doc.isObject()) {
        Logger::instance().error("Parsed JSON is not an object");
        return QJsonObject();
    }

    QJsonObject response = doc.object();

    // Validate it's a world plan
    if (response.contains("world_name") && response.contains("models")) {
        return response;
    }

    Logger::instance().warning("JSON doesn't match expected world plan format");
    return QJsonObject();
}

} // namespace Burma
