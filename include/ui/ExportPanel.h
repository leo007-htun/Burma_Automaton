#ifndef BURMA_EXPORTPANEL_H
#define BURMA_EXPORTPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>

namespace Burma {

class ExportPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ExportPanel(QWidget *parent = nullptr);
    ~ExportPanel() override = default;

    void showExportDialog();

signals:
    void exportRequested(const QString &outputPath, double resolution, int width, int height);

private slots:
    void onBrowseClicked();
    void onExportClicked();

public slots:
    void setExportProgress(int percentage);
    void setExportStatus(const QString &status);

private:
    void setupUi();

    QLineEdit *m_outputPath;
    QPushButton *m_browseButton;
    QDoubleSpinBox *m_resolutionSpin;
    QSpinBox *m_widthSpin;
    QSpinBox *m_heightSpin;
    QPushButton *m_exportButton;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
};

} // namespace Burma

#endif // BURMA_EXPORTPANEL_H
