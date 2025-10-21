#include "ui/PromptPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSplitter>

namespace Burma {

PromptPanel::PromptPanel(QWidget *parent)
    : QWidget(parent)
    , m_promptInput(nullptr)
    , m_submitButton(nullptr)
    , m_clearButton(nullptr)
    , m_templateCombo(nullptr)
    , m_historyList(nullptr)
{
    setupUi();
    setupTemplates();
}

void PromptPanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Template selection
    QGroupBox *templateGroup = new QGroupBox(tr("Templates"), this);
    QVBoxLayout *templateLayout = new QVBoxLayout(templateGroup);

    m_templateCombo = new QComboBox(this);
    m_templateCombo->setToolTip(tr("Select a template prompt to get started"));
    templateLayout->addWidget(m_templateCombo);

    mainLayout->addWidget(templateGroup);

    // Prompt input
    QGroupBox *promptGroup = new QGroupBox(tr("Describe Your World"), this);
    QVBoxLayout *promptLayout = new QVBoxLayout(promptGroup);

    m_promptInput = new QTextEdit(this);
    m_promptInput->setPlaceholderText(
        tr("Describe the simulation world you want to create...\n\n"
           "Examples:\n"
           "- Create a warehouse with shelving units and an AMR robot\n"
           "- Build a factory floor with two conveyor belts and industrial lighting\n"
           "- Generate an outdoor environment with trees, rocks, and uneven terrain")
    );
    m_promptInput->setMinimumHeight(150);
    promptLayout->addWidget(m_promptInput);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_submitButton = new QPushButton(tr("Generate World"), this);
    m_submitButton->setToolTip(tr("Process prompt and generate Gazebo world"));
    m_submitButton->setDefault(true);

    m_clearButton = new QPushButton(tr("Clear"), this);
    m_clearButton->setToolTip(tr("Clear prompt text"));

    buttonLayout->addWidget(m_submitButton);
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addStretch();

    promptLayout->addLayout(buttonLayout);
    mainLayout->addWidget(promptGroup);

    // History
    QGroupBox *historyGroup = new QGroupBox(tr("History"), this);
    QVBoxLayout *historyLayout = new QVBoxLayout(historyGroup);

    m_historyList = new QListWidget(this);
    m_historyList->setToolTip(tr("Click to reuse a previous prompt"));
    historyLayout->addWidget(m_historyList);

    mainLayout->addWidget(historyGroup);

    // Connections
    connect(m_submitButton, &QPushButton::clicked, this, &PromptPanel::onSubmitClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &PromptPanel::onClearClicked);
    connect(m_templateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PromptPanel::onTemplateSelected);
    connect(m_historyList, &QListWidget::itemClicked,
            this, &PromptPanel::onHistoryItemClicked);

    // Enable submit on Ctrl+Enter
    m_promptInput->installEventFilter(this);
}

void PromptPanel::setupTemplates()
{
    m_templateCombo->addItem(tr("-- Select Template --"), "");
    m_templateCombo->addItem(tr("Warehouse Environment"),
        "Create a warehouse environment with shelving units, pallets, and an AMR robot. "
        "Add loading docks and adequate lighting.");
    m_templateCombo->addItem(tr("Factory Floor"),
        "Build a factory floor with two conveyor belts, robotic arms, and industrial lighting. "
        "Include safety barriers and workstations.");
    m_templateCombo->addItem(tr("Outdoor Terrain"),
        "Generate an outdoor environment with varied terrain, trees, rocks, and natural lighting. "
        "Include hills and pathways.");
    m_templateCombo->addItem(tr("Office Building"),
        "Create an office building interior with desks, chairs, computers, and hallways. "
        "Add multiple rooms and doorways.");
    m_templateCombo->addItem(tr("Urban Street"),
        "Build an urban street scene with buildings, sidewalks, street lights, and traffic elements. "
        "Include crosswalks and signage.");
    m_templateCombo->addItem(tr("Research Lab"),
        "Create a robotics research lab with equipment racks, testing areas, and various robots. "
        "Include workbenches and tool storage.");
}

void PromptPanel::onSubmitClicked()
{
    QString prompt = m_promptInput->toPlainText().trimmed();

    if (prompt.isEmpty()) {
        return;
    }

    addToHistory(prompt);
    emit promptSubmitted(prompt);
}

void PromptPanel::onClearClicked()
{
    m_promptInput->clear();
    m_promptInput->setFocus();
}

void PromptPanel::onTemplateSelected(int index)
{
    if (index <= 0) {
        return;
    }

    QString templateText = m_templateCombo->itemData(index).toString();
    m_promptInput->setPlainText(templateText);
    m_promptInput->setFocus();
}

void PromptPanel::onHistoryItemClicked(QListWidgetItem *item)
{
    if (item) {
        m_promptInput->setPlainText(item->text());
        m_promptInput->setFocus();
    }
}

void PromptPanel::addToHistory(const QString &prompt)
{
    // Avoid duplicates
    if (m_history.contains(prompt)) {
        return;
    }

    m_history.prepend(prompt);
    m_historyList->insertItem(0, prompt);

    // Limit history to 20 items
    while (m_history.size() > 20) {
        m_history.removeLast();
        delete m_historyList->takeItem(m_historyList->count() - 1);
    }
}

} // namespace Burma
