#include "ui/PropertyEditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFormLayout>
#include <QHeaderView>

namespace Burma {

PropertyEditor::PropertyEditor(QWidget *parent)
    : QWidget(parent)
    , m_propertyTree(nullptr)
    , m_currentColor(255, 255, 255)
{
    setupUi();
}

void PropertyEditor::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Entity info
    QLabel *infoLabel = new QLabel(tr("Select an entity to edit properties"), this);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { color: gray; font-style: italic; }");
    mainLayout->addWidget(infoLabel);

    // Transform section
    QWidget *transformWidget = createTransformWidget();
    mainLayout->addWidget(transformWidget);

    // Visual properties section
    QWidget *visualWidget = createVisualWidget();
    mainLayout->addWidget(visualWidget);

    // Property tree for other properties
    QGroupBox *propertiesGroup = new QGroupBox(tr("All Properties"), this);
    QVBoxLayout *propertiesLayout = new QVBoxLayout(propertiesGroup);

    m_propertyTree = new QTreeWidget(this);
    m_propertyTree->setHeaderLabels({tr("Property"), tr("Value")});
    m_propertyTree->setAlternatingRowColors(true);
    m_propertyTree->header()->setStretchLastSection(true);
    m_propertyTree->setColumnWidth(0, 150);

    propertiesLayout->addWidget(m_propertyTree);
    mainLayout->addWidget(propertiesGroup);

    mainLayout->addStretch();

    // Connections
    connect(m_propertyTree, &QTreeWidget::itemChanged,
            this, &PropertyEditor::onPropertyItemChanged);
}

QWidget* PropertyEditor::createTransformWidget()
{
    QGroupBox *transformGroup = new QGroupBox(tr("Transform"), this);
    QFormLayout *formLayout = new QFormLayout(transformGroup);

    // Position
    QWidget *posWidget = new QWidget();
    QHBoxLayout *posLayout = new QHBoxLayout(posWidget);
    posLayout->setContentsMargins(0, 0, 0, 0);

    m_posXSpin = new QDoubleSpinBox();
    m_posXSpin->setRange(-1000.0, 1000.0);
    m_posXSpin->setPrefix("X: ");
    m_posXSpin->setSuffix(" m");

    m_posYSpin = new QDoubleSpinBox();
    m_posYSpin->setRange(-1000.0, 1000.0);
    m_posYSpin->setPrefix("Y: ");
    m_posYSpin->setSuffix(" m");

    m_posZSpin = new QDoubleSpinBox();
    m_posZSpin->setRange(-1000.0, 1000.0);
    m_posZSpin->setPrefix("Z: ");
    m_posZSpin->setSuffix(" m");

    posLayout->addWidget(m_posXSpin);
    posLayout->addWidget(m_posYSpin);
    posLayout->addWidget(m_posZSpin);

    formLayout->addRow(tr("Position:"), posWidget);

    // Rotation
    QWidget *rotWidget = new QWidget();
    QHBoxLayout *rotLayout = new QHBoxLayout(rotWidget);
    rotLayout->setContentsMargins(0, 0, 0, 0);

    m_rotRSpin = new QDoubleSpinBox();
    m_rotRSpin->setRange(-180.0, 180.0);
    m_rotRSpin->setPrefix("R: ");
    m_rotRSpin->setSuffix("°");

    m_rotPSpin = new QDoubleSpinBox();
    m_rotPSpin->setRange(-180.0, 180.0);
    m_rotPSpin->setPrefix("P: ");
    m_rotPSpin->setSuffix("°");

    m_rotYawSpin = new QDoubleSpinBox();
    m_rotYawSpin->setRange(-180.0, 180.0);
    m_rotYawSpin->setPrefix("Y: ");
    m_rotYawSpin->setSuffix("°");

    rotLayout->addWidget(m_rotRSpin);
    rotLayout->addWidget(m_rotPSpin);
    rotLayout->addWidget(m_rotYawSpin);

    formLayout->addRow(tr("Rotation:"), rotWidget);

    // Scale
    QWidget *scaleWidget = new QWidget();
    QHBoxLayout *scaleLayout = new QHBoxLayout(scaleWidget);
    scaleLayout->setContentsMargins(0, 0, 0, 0);

    m_scaleXSpin = new QDoubleSpinBox();
    m_scaleXSpin->setRange(0.01, 100.0);
    m_scaleXSpin->setValue(1.0);
    m_scaleXSpin->setPrefix("X: ");

    m_scaleYSpin = new QDoubleSpinBox();
    m_scaleYSpin->setRange(0.01, 100.0);
    m_scaleYSpin->setValue(1.0);
    m_scaleYSpin->setPrefix("Y: ");

    m_scaleZSpin = new QDoubleSpinBox();
    m_scaleZSpin->setRange(0.01, 100.0);
    m_scaleZSpin->setValue(1.0);
    m_scaleZSpin->setPrefix("Z: ");

    scaleLayout->addWidget(m_scaleXSpin);
    scaleLayout->addWidget(m_scaleYSpin);
    scaleLayout->addWidget(m_scaleZSpin);

    formLayout->addRow(tr("Scale:"), scaleWidget);

    // Apply button
    QPushButton *applyButton = new QPushButton(tr("Apply Transform"), this);
    connect(applyButton, &QPushButton::clicked, this, &PropertyEditor::onApplyTransform);
    formLayout->addRow("", applyButton);

    return transformGroup;
}

QWidget* PropertyEditor::createVisualWidget()
{
    QGroupBox *visualGroup = new QGroupBox(tr("Visual Properties"), this);
    QFormLayout *formLayout = new QFormLayout(visualGroup);

    // Color picker
    QWidget *colorWidget = new QWidget();
    QHBoxLayout *colorLayout = new QHBoxLayout(colorWidget);
    colorLayout->setContentsMargins(0, 0, 0, 0);

    m_colorButton = new QPushButton(tr("Choose Color"), this);
    m_colorButton->setStyleSheet(
        QString("QPushButton { background-color: %1; }").arg(m_currentColor.name()));
    connect(m_colorButton, &QPushButton::clicked, this, &PropertyEditor::onColorButtonClicked);

    colorLayout->addWidget(m_colorButton);
    colorLayout->addStretch();

    formLayout->addRow(tr("Color:"), colorWidget);

    return visualGroup;
}


void PropertyEditor::setSelectedEntity(const QString &entityName)
{
    m_selectedEntity = entityName;
    updatePropertyTree();
}

void PropertyEditor::clearSelection()
{
    m_selectedEntity.clear();
    m_propertyTree->clear();

    // Reset transform values
    m_posXSpin->setValue(0.0);
    m_posYSpin->setValue(0.0);
    m_posZSpin->setValue(0.0);
    m_rotRSpin->setValue(0.0);
    m_rotPSpin->setValue(0.0);
    m_rotYawSpin->setValue(0.0);
    m_scaleXSpin->setValue(1.0);
    m_scaleYSpin->setValue(1.0);
    m_scaleZSpin->setValue(1.0);
}

void PropertyEditor::updatePropertyTree()
{
    m_propertyTree->clear();

    if (m_selectedEntity.isEmpty()) {
        return;
    }

    // Add property groups
    addPropertyGroup("Transform");
    addProperty("Transform", "Position", "0, 0, 0");
    addProperty("Transform", "Rotation", "0, 0, 0");
    addProperty("Transform", "Scale", "1, 1, 1");

    addPropertyGroup("Visual");
    addProperty("Visual", "Color", m_currentColor.name());
    addProperty("Visual", "Material", "Default");

    addPropertyGroup("Physics");
    addProperty("Physics", "Mass", 1.0);
    addProperty("Physics", "Static", false);
}

void PropertyEditor::addPropertyGroup(const QString &groupName)
{
    QTreeWidgetItem *group = new QTreeWidgetItem(m_propertyTree);
    group->setText(0, groupName);
    group->setFlags(group->flags() & ~Qt::ItemIsEditable);
    group->setExpanded(true);
    QFont font = group->font(0);
    font.setBold(true);
    group->setFont(0, font);
}

void PropertyEditor::addProperty(const QString &group, const QString &name,
                                const QVariant &value, bool editable)
{
    QTreeWidgetItem *groupItem = nullptr;
    for (int i = 0; i < m_propertyTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_propertyTree->topLevelItem(i);
        if (item->text(0) == group) {
            groupItem = item;
            break;
        }
    }

    if (!groupItem) {
        return;
    }

    QTreeWidgetItem *propItem = new QTreeWidgetItem(groupItem);
    propItem->setText(0, name);
    propItem->setText(1, value.toString());

    if (editable) {
        propItem->setFlags(propItem->flags() | Qt::ItemIsEditable);
    } else {
        propItem->setFlags(propItem->flags() & ~Qt::ItemIsEditable);
    }
}

void PropertyEditor::onPropertyItemChanged(QTreeWidgetItem *item, int column)
{
    if (column != 1 || m_selectedEntity.isEmpty()) {
        return;
    }

    QString propertyName = item->text(0);
    QVariant value = item->text(1);

    emit propertyChanged(m_selectedEntity, propertyName, value);
}

void PropertyEditor::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_currentColor, this, tr("Select Entity Color"));

    if (color.isValid()) {
        m_currentColor = color;
        m_colorButton->setStyleSheet(
            QString("QPushButton { background-color: %1; }").arg(color.name()));

        if (!m_selectedEntity.isEmpty()) {
            emit propertyChanged(m_selectedEntity, "Color", color);
        }
    }
}

void PropertyEditor::onApplyTransform()
{
    if (m_selectedEntity.isEmpty()) {
        return;
    }

    // Emit property changes for transform
    emit propertyChanged(m_selectedEntity, "Position.X", m_posXSpin->value());
    emit propertyChanged(m_selectedEntity, "Position.Y", m_posYSpin->value());
    emit propertyChanged(m_selectedEntity, "Position.Z", m_posZSpin->value());

    emit propertyChanged(m_selectedEntity, "Rotation.Roll", m_rotRSpin->value());
    emit propertyChanged(m_selectedEntity, "Rotation.Pitch", m_rotPSpin->value());
    emit propertyChanged(m_selectedEntity, "Rotation.Yaw", m_rotYawSpin->value());

    emit propertyChanged(m_selectedEntity, "Scale.X", m_scaleXSpin->value());
    emit propertyChanged(m_selectedEntity, "Scale.Y", m_scaleYSpin->value());
    emit propertyChanged(m_selectedEntity, "Scale.Z", m_scaleZSpin->value());
}

} // namespace Burma
