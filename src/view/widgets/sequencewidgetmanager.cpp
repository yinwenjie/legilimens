#include "view/widgets/sequencewidgetmanager.h"
#include <QVBoxLayout>

SequenceWidgetManager::SequenceWidgetManager(QWidget *parent)
    : BaseWidgetManager(parent)
{
}

void SequenceWidgetManager::setupContentWidget()
{
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    // Basic layout setup - will be populated with actual widgets later
}

void SequenceWidgetManager::setupConnections()
{
    // Will be implemented when data management is decided
}

void SequenceWidgetManager::updateContent()
{
    // Will be implemented when data management is decided
} 