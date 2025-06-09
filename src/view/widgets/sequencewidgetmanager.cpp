#include "view/widgets/sequencewidgetmanager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

SequenceWidgetManager::SequenceWidgetManager(QWidget *parent)
    : BaseWidgetManager(parent)
{
}

void SequenceWidgetManager::setupContentWidget()
{
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    // Add a placeholder label
    QLabel *label = new QLabel("Sequence View", contentWidget);
    layout->addWidget(label);
}

void SequenceWidgetManager::setupConnections()
{
    // Setup any necessary connections here
}

void SequenceWidgetManager::updateContent()
{
    // Update sequence content here
}

void SequenceWidgetManager::clearContent()
{
    // Clear sequence widget content
    qDebug() << "Cleared sequence widget content";
} 