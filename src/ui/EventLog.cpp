#include "ui/EventLog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDateTime>
#include <QScrollBar>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

namespace Burma {

EventLog::EventLog(QWidget *parent)
    : QWidget(parent)
    , m_logText(nullptr)
    , m_clearButton(nullptr)
    , m_saveButton(nullptr)
    , m_showDebug(nullptr)
    , m_showInfo(nullptr)
    , m_showWarning(nullptr)
    , m_showError(nullptr)
{
    setupUi();
}

void EventLog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Log text display
    m_logText = new QTextEdit(this);
    m_logText->setReadOnly(true);
    m_logText->setLineWrapMode(QTextEdit::NoWrap);
    m_logText->setStyleSheet(
        "QTextEdit { "
        "  background-color: #1e1e1e; "
        "  color: #d4d4d4; "
        "  font-family: 'Courier New', monospace; "
        "  font-size: 10pt; "
        "}"
    );

    mainLayout->addWidget(m_logText);

    // Control panel
    QHBoxLayout *controlLayout = new QHBoxLayout();

    // Filter checkboxes
    m_showDebug = new QCheckBox(tr("Debug"), this);
    m_showDebug->setChecked(true);
    m_showInfo = new QCheckBox(tr("Info"), this);
    m_showInfo->setChecked(true);
    m_showWarning = new QCheckBox(tr("Warning"), this);
    m_showWarning->setChecked(true);
    m_showError = new QCheckBox(tr("Error"), this);
    m_showError->setChecked(true);

    controlLayout->addWidget(m_showDebug);
    controlLayout->addWidget(m_showInfo);
    controlLayout->addWidget(m_showWarning);
    controlLayout->addWidget(m_showError);
    controlLayout->addStretch();

    // Action buttons
    m_clearButton = new QPushButton(tr("Clear"), this);
    m_saveButton = new QPushButton(tr("Save Log"), this);

    controlLayout->addWidget(m_clearButton);
    controlLayout->addWidget(m_saveButton);

    mainLayout->addLayout(controlLayout);

    // Connections
    connect(m_clearButton, &QPushButton::clicked, this, &EventLog::onClearClicked);
    connect(m_saveButton, &QPushButton::clicked, this, &EventLog::onSaveLogClicked);
    connect(m_showDebug, &QCheckBox::stateChanged, this, &EventLog::onFilterChanged);
    connect(m_showInfo, &QCheckBox::stateChanged, this, &EventLog::onFilterChanged);
    connect(m_showWarning, &QCheckBox::stateChanged, this, &EventLog::onFilterChanged);
    connect(m_showError, &QCheckBox::stateChanged, this, &EventLog::onFilterChanged);
}

void EventLog::appendMessage(const QString &message, const QString &level)
{
    // Check filter
    QString levelUpper = level.toUpper();
    if ((levelUpper == "DEBUG" && !m_showDebug->isChecked()) ||
        (levelUpper == "INFO" && !m_showInfo->isChecked()) ||
        (levelUpper == "WARNING" && !m_showWarning->isChecked()) ||
        (levelUpper == "ERROR" && !m_showError->isChecked())) {
        return;
    }

    QString formattedMsg = formatMessage(message, levelUpper);
    m_logText->append(formattedMsg);

    // Auto-scroll to bottom
    if (m_autoScroll) {
        QScrollBar *scrollBar = m_logText->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

void EventLog::clear()
{
    m_logText->clear();
}

QString EventLog::formatMessage(const QString &message, const QString &level)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString color = getLevelColor(level);

    return QString("<span style='color: gray;'>%1</span> "
                  "<span style='color: %2;'>[%3]</span> "
                  "<span style='color: #d4d4d4;'>%4</span>")
        .arg(timestamp)
        .arg(color)
        .arg(level.leftJustified(7))
        .arg(message.toHtmlEscaped());
}

QString EventLog::getLevelColor(const QString &level)
{
    if (level == "DEBUG") {
        return "#808080"; // Gray
    } else if (level == "INFO") {
        return "#4ec9b0"; // Cyan
    } else if (level == "WARNING") {
        return "#dcdcaa"; // Yellow
    } else if (level == "ERROR") {
        return "#f48771"; // Red
    }
    return "#d4d4d4"; // Default
}

void EventLog::onClearClicked()
{
    clear();
}

void EventLog::onSaveLogClicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Event Log"),
        QString("burma_log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")),
        tr("Text Files (*.txt);;All Files (*)")
    );

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << m_logText->toPlainText();
            file.close();
            QMessageBox::information(this, tr("Log Saved"), tr("Event log saved to: %1").arg(fileName));
        } else {
            QMessageBox::warning(this, tr("Save Failed"), tr("Failed to save log file"));
        }
    }
}

void EventLog::onFilterChanged()
{
    // In a real implementation, we would re-filter the log
    // For now, this just affects future messages
}

} // namespace Burma
