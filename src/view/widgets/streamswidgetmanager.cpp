#include "view/widgets/streamswidgetmanager.h"
#include "model/streamtreemodel.h"
#include <QVBoxLayout>

StreamsWidgetManager::StreamsWidgetManager(QWidget *parent)
    : BaseWidgetManager(parent)
    , treeView(nullptr)
    , streamModel(nullptr)
{
}

void StreamsWidgetManager::setupContentWidget()
{
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->setContentsMargins(0, 0, 0, 0);  // Remove margins to maximize space

    // Create tree view
    treeView = new QTreeView(contentWidget);
    treeView->setHeaderHidden(false);  // Show the header
    treeView->setAlternatingRowColors(true);  // Alternate row colors for better readability
    treeView->setAnimated(true);  // Animate expansion/collapse
    treeView->setIndentation(20);  // Set indentation for tree levels
    
    // Create and set the model
    streamModel = new StreamTreeModel(this);
    treeView->setModel(streamModel);
    
    // Expand the root item by default
    treeView->expandToDepth(0);
    
    // Add the tree view to the layout
    layout->addWidget(treeView);
}

void StreamsWidgetManager::setupConnections()
{
    // Connect any necessary signals/slots here
    // For example, when a stream is selected:
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](const QItemSelection &selected, const QItemSelection &deselected) {
        // Handle selection changes here
        Q_UNUSED(deselected);
        if (!selected.indexes().isEmpty()) {
            QModelIndex index = selected.indexes().first();
            // Handle the selected item
        }
    });
}

void StreamsWidgetManager::updateContent()
{
    // Update the model with new stream data
    if (streamModel) {
        // TODO: Update the model with actual stream data
        // For now, the example data in setupModelData() will be shown
    }
} 