#include "ui/AssetBrowser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

namespace Burma {

AssetBrowser::AssetBrowser(QWidget *parent)
    : QWidget(parent)
    , m_searchInput(nullptr)
    , m_searchButton(nullptr)
    , m_refreshButton(nullptr)
    , m_assetList(nullptr)
    , m_downloadButton(nullptr)
    , m_statusLabel(nullptr)
    , m_progressBar(nullptr)
{
    setupUi();
}

void AssetBrowser::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Search bar
    QHBoxLayout *searchLayout = new QHBoxLayout();

    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText(tr("Search Gazebo Fuel assets..."));

    m_searchButton = new QPushButton(tr("Search"), this);
    m_refreshButton = new QPushButton(tr("Refresh"), this);

    searchLayout->addWidget(m_searchInput);
    searchLayout->addWidget(m_searchButton);
    searchLayout->addWidget(m_refreshButton);

    mainLayout->addLayout(searchLayout);

    // Asset list
    QGroupBox *assetsGroup = new QGroupBox(tr("Available Assets"), this);
    QVBoxLayout *assetsLayout = new QVBoxLayout(assetsGroup);

    m_assetList = new QListWidget(this);
    m_assetList->setIconSize(QSize(64, 64));
    m_assetList->setViewMode(QListWidget::IconMode);
    m_assetList->setResizeMode(QListWidget::Adjust);
    m_assetList->setMovement(QListWidget::Static);
    m_assetList->setSpacing(10);

    assetsLayout->addWidget(m_assetList);
    mainLayout->addWidget(assetsGroup);

    // Download section
    QHBoxLayout *downloadLayout = new QHBoxLayout();
    m_downloadButton = new QPushButton(tr("Download Selected"), this);
    m_downloadButton->setEnabled(false);
    downloadLayout->addWidget(m_downloadButton);
    downloadLayout->addStretch();

    mainLayout->addLayout(downloadLayout);

    // Status and progress
    m_statusLabel = new QLabel(tr("Ready"), this);
    m_statusLabel->setStyleSheet("QLabel { color: gray; font-style: italic; }");
    mainLayout->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setRange(0, 100);
    mainLayout->addWidget(m_progressBar);

    // Connections
    connect(m_searchButton, &QPushButton::clicked, this, &AssetBrowser::onSearchClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &AssetBrowser::onRefreshClicked);
    connect(m_assetList, &QListWidget::itemClicked, this, &AssetBrowser::onAssetItemClicked);
    connect(m_downloadButton, &QPushButton::clicked, this, &AssetBrowser::onDownloadClicked);

    // Connect Enter key in search box
    connect(m_searchInput, &QLineEdit::returnPressed, this, &AssetBrowser::onSearchClicked);

    // Add some default popular assets
    addAsset("Ground Plane", "https://fuel.gazebosim.org/1.0/OpenRobotics/models/Ground Plane", "");
    addAsset("Sun", "https://fuel.gazebosim.org/1.0/OpenRobotics/models/Sun", "");
    addAsset("Warehouse", "https://fuel.gazebosim.org/1.0/OpenRobotics/models/Warehouse", "");
}

void AssetBrowser::searchAssets(const QString &query)
{
    if (query.isEmpty()) {
        return;
    }

    m_statusLabel->setText(tr("Searching for: %1...").arg(query));
    m_assetList->clear();

    // TODO: Integrate with FuelFetcher to actually search Gazebo Fuel
    // For now, this is a placeholder
}

void AssetBrowser::addAsset(const QString &name, const QString &url, const QString &thumbnail)
{
    QListWidgetItem *item = new QListWidgetItem(name, m_assetList);
    item->setData(Qt::UserRole, url);
    item->setToolTip(url);

    // TODO: Load thumbnail image if provided
    // For now, use a default icon
}

void AssetBrowser::clearAssets()
{
    m_assetList->clear();
}

void AssetBrowser::setDownloadProgress(int percentage)
{
    if (percentage < 0) {
        m_progressBar->setVisible(false);
        return;
    }

    m_progressBar->setVisible(true);
    m_progressBar->setValue(percentage);

    if (percentage >= 100) {
        m_statusLabel->setText(tr("Download complete"));
    } else {
        m_statusLabel->setText(tr("Downloading... %1%").arg(percentage));
    }
}

void AssetBrowser::onSearchClicked()
{
    QString query = m_searchInput->text().trimmed();
    searchAssets(query);
}

void AssetBrowser::onAssetItemClicked(QListWidgetItem *item)
{
    if (item) {
        m_selectedAsset = item->data(Qt::UserRole).toString();
        m_downloadButton->setEnabled(true);
        emit assetSelected(m_selectedAsset);
    }
}

void AssetBrowser::onDownloadClicked()
{
    if (!m_selectedAsset.isEmpty()) {
        m_statusLabel->setText(tr("Downloading asset..."));
        emit assetDownloadRequested(m_selectedAsset);
    }
}

void AssetBrowser::onRefreshClicked()
{
    m_statusLabel->setText(tr("Refreshing asset list..."));
    // TODO: Refresh the asset list from Gazebo Fuel
}

} // namespace Burma
