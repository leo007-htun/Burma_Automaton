#include "ui/ExportPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>

namespace Burma {

ExportPanel::ExportPanel(QWidget *parent)
    : QWidget(parent)
    , m_outputPath(nullptr)
    , m_browseButton(nullptr)
    , m_resolutionSpin(nullptr)
    , m_widthSpin(nullptr)
    , m_heightSpin(nullptr)
    , m_exportButton(nullptr)
    , m_statusLabel(nullptr)
    , m_progressBar(nullptr)
{
    setupUi();
}

void ExportPanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Export settings
    QGroupBox *settingsGroup = new QGroupBox(tr("RViz Map Export Settings"), this);
    QFormLayout *formLayout = new QFormLayout(settingsGroup);

    // Output path
    QWidget *pathWidget = new QWidget();
    QHBoxLayout *pathLayout = new QHBoxLayout(pathWidget);
    pathLayout->setContentsMargins(0, 0, 0, 0);

    m_outputPath = new QLineEdit(this);
    m_outputPath->setPlaceholderText(tr("Select output directory..."));

    m_browseButton = new QPushButton(tr("Browse"), this);

    pathLayout->addWidget(m_outputPath);
    pathLayout->addWidget(m_browseButton);

    formLayout->addRow(tr("Output Path:"), pathWidget);

    // Resolution
    m_resolutionSpin = new QDoubleSpinBox(this);
    m_resolutionSpin->setRange(0.001, 10.0);
    m_resolutionSpin->setValue(0.05);
    m_resolutionSpin->setSingleStep(0.01);
    m_resolutionSpin->setSuffix(" m/pixel");
    m_resolutionSpin->setToolTip(tr("Map resolution in meters per pixel"));

    formLayout->addRow(tr("Resolution:"), m_resolutionSpin);

    // Width
    m_widthSpin = new QSpinBox(this);
    m_widthSpin->setRange(100, 10000);
    m_widthSpin->setValue(2000);
    m_widthSpin->setSuffix(" px");
    m_widthSpin->setToolTip(tr("Map image width in pixels"));

    formLayout->addRow(tr("Width:"), m_widthSpin);

    // Height
    m_heightSpin = new QSpinBox(this);
    m_heightSpin->setRange(100, 10000);
    m_heightSpin->setValue(2000);
    m_heightSpin->setSuffix(" px");
    m_heightSpin->setToolTip(tr("Map image height in pixels"));

    formLayout->addRow(tr("Height:"), m_heightSpin);

    mainLayout->addWidget(settingsGroup);

    // Export button
    m_exportButton = new QPushButton(tr("Export to RViz Format"), this);
    m_exportButton->setToolTip(tr("Export current world as .pgm and .yaml files for RViz"));
    mainLayout->addWidget(m_exportButton);

    // Status and progress
    m_statusLabel = new QLabel(tr("Ready to export"), this);
    m_statusLabel->setStyleSheet("QLabel { color: gray; font-style: italic; }");
    mainLayout->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setRange(0, 100);
    mainLayout->addWidget(m_progressBar);

    // Info text
    QLabel *infoLabel = new QLabel(
        tr("<b>Export Information:</b><br>"
           "This will create two files:<br>"
           "• <b>.pgm</b> - Occupancy grid image<br>"
           "• <b>.yaml</b> - Map metadata for RViz<br><br>"
           "Uses the Gazebo-map-to-Rviz converter."),
        this
    );
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { color: gray; }");
    mainLayout->addWidget(infoLabel);

    mainLayout->addStretch();

    // Connections
    connect(m_browseButton, &QPushButton::clicked, this, &ExportPanel::onBrowseClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &ExportPanel::onExportClicked);
}

void ExportPanel::showExportDialog()
{
    onBrowseClicked();
}

void ExportPanel::setExportProgress(int percentage)
{
    if (percentage < 0) {
        m_progressBar->setVisible(false);
        m_exportButton->setEnabled(true);
        return;
    }

    m_progressBar->setVisible(true);
    m_progressBar->setValue(percentage);
    m_exportButton->setEnabled(false);

    if (percentage >= 100) {
        m_statusLabel->setText(tr("Export complete!"));
        m_exportButton->setEnabled(true);
    } else {
        m_statusLabel->setText(tr("Exporting... %1%").arg(percentage));
    }
}

void ExportPanel::setExportStatus(const QString &status)
{
    m_statusLabel->setText(status);
}

void ExportPanel::onBrowseClicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(
        this,
        tr("Select Output Directory for RViz Map"),
        m_outputPath->text()
    );

    if (!dirPath.isEmpty()) {
        m_outputPath->setText(dirPath);
    }
}

void ExportPanel::onExportClicked()
{
    QString outputPath = m_outputPath->text().trimmed();

    if (outputPath.isEmpty()) {
        QMessageBox::warning(this, tr("No Output Path"),
                           tr("Please select an output directory for the exported map."));
        return;
    }

    double resolution = m_resolutionSpin->value();
    int width = m_widthSpin->value();
    int height = m_heightSpin->value();

    m_statusLabel->setText(tr("Starting export..."));
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);

    emit exportRequested(outputPath, resolution, width, height);
}

} // namespace Burma
