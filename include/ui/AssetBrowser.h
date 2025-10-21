#ifndef BURMA_ASSETBROWSER_H
#define BURMA_ASSETBROWSER_H

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>

namespace Burma {

class AssetBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit AssetBrowser(QWidget *parent = nullptr);
    ~AssetBrowser() override = default;

signals:
    void assetSelected(const QString &assetUrl);
    void assetDownloadRequested(const QString &assetUrl);

public slots:
    void searchAssets(const QString &query);
    void addAsset(const QString &name, const QString &url, const QString &thumbnail);
    void clearAssets();
    void setDownloadProgress(int percentage);

private slots:
    void onSearchClicked();
    void onAssetItemClicked(QListWidgetItem *item);
    void onDownloadClicked();
    void onRefreshClicked();

private:
    void setupUi();

    QLineEdit *m_searchInput;
    QPushButton *m_searchButton;
    QPushButton *m_refreshButton;
    QListWidget *m_assetList;
    QPushButton *m_downloadButton;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;

    QString m_selectedAsset;
};

} // namespace Burma

#endif // BURMA_ASSETBROWSER_H
