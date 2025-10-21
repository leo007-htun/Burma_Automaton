#ifndef BURMA_EVENTLOG_H
#define BURMA_EVENTLOG_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>

namespace Burma {

class EventLog : public QWidget
{
    Q_OBJECT

public:
    explicit EventLog(QWidget *parent = nullptr);
    ~EventLog() override = default;

public slots:
    void appendMessage(const QString &message, const QString &level = "INFO");
    void clear();

private slots:
    void onClearClicked();
    void onSaveLogClicked();
    void onFilterChanged();

private:
    void setupUi();
    QString formatMessage(const QString &message, const QString &level);
    QString getLevelColor(const QString &level);

    QTextEdit *m_logText;
    QPushButton *m_clearButton;
    QPushButton *m_saveButton;
    QCheckBox *m_showDebug;
    QCheckBox *m_showInfo;
    QCheckBox *m_showWarning;
    QCheckBox *m_showError;

    bool m_autoScroll = true;
};

} // namespace Burma

#endif // BURMA_EVENTLOG_H
