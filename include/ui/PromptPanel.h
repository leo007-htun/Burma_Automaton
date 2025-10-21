#ifndef BURMA_PROMPTPANEL_H
#define BURMA_PROMPTPANEL_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>

namespace Burma {

class PromptPanel : public QWidget
{
    Q_OBJECT

public:
    explicit PromptPanel(QWidget *parent = nullptr);
    ~PromptPanel() override = default;

signals:
    void promptSubmitted(const QString &prompt);

private slots:
    void onSubmitClicked();
    void onClearClicked();
    void onTemplateSelected(int index);
    void onHistoryItemClicked(QListWidgetItem *item);

private:
    void setupUi();
    void setupTemplates();
    void addToHistory(const QString &prompt);

    QTextEdit *m_promptInput;
    QPushButton *m_submitButton;
    QPushButton *m_clearButton;
    QComboBox *m_templateCombo;
    QListWidget *m_historyList;

    QStringList m_history;
};

} // namespace Burma

#endif // BURMA_PROMPTPANEL_H
