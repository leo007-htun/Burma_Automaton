#ifndef BURMA_PROPERTYEDITOR_H
#define BURMA_PROPERTYEDITOR_H

#include <QWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QColorDialog>

namespace Burma {

class PropertyEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyEditor(QWidget *parent = nullptr);
    ~PropertyEditor() override = default;

    void setSelectedEntity(const QString &entityName);
    void clearSelection();

signals:
    void propertyChanged(const QString &entityName, const QString &property, const QVariant &value);

private slots:
    void onPropertyItemChanged(QTreeWidgetItem *item, int column);
    void onColorButtonClicked();
    void onApplyTransform();

private:
    void setupUi();
    QWidget* createTransformWidget();
    QWidget* createVisualWidget();

    void updatePropertyTree();
    void addPropertyGroup(const QString &groupName);
    void addProperty(const QString &group, const QString &name, const QVariant &value, bool editable = true);

    QTreeWidget *m_propertyTree;
    QString m_selectedEntity;

    // Transform controls
    QDoubleSpinBox *m_posXSpin;
    QDoubleSpinBox *m_posYSpin;
    QDoubleSpinBox *m_posZSpin;
    QDoubleSpinBox *m_rotRSpin;
    QDoubleSpinBox *m_rotPSpin;
    QDoubleSpinBox *m_rotYawSpin;
    QDoubleSpinBox *m_scaleXSpin;
    QDoubleSpinBox *m_scaleYSpin;
    QDoubleSpinBox *m_scaleZSpin;

    // Visual controls
    QPushButton *m_colorButton;
    QColor m_currentColor;
};

} // namespace Burma

#endif // BURMA_PROPERTYEDITOR_H
