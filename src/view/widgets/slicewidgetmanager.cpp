#include "view/widgets/slicewidgetmanager.h"
#include "model/slicetreemodel.h"
#include "controller/controller.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

SliceWidgetManager::SliceWidgetManager(QWidget *parent)
    : BaseWidgetManager(parent)
    , treeView(nullptr)
    , sliceModel(nullptr)
    , connectedController(nullptr)
{
}

void SliceWidgetManager::setupContentWidget()
{
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create tree view
    treeView = new QTreeView(contentWidget);
    treeView->setHeaderHidden(false);
    treeView->setAlternatingRowColors(true);
    treeView->setAnimated(true);
    treeView->setIndentation(20);
    
    // Create and set the model
    sliceModel = new SliceTreeModel(this);
    treeView->setModel(sliceModel);
    
    // Expand the root item by default
    treeView->expandToDepth(0);
    
    // Add the tree view to the layout
    layout->addWidget(treeView);
}

void SliceWidgetManager::setupConnections()
{
    // Connect tree view selection changes
    if (treeView && treeView->selectionModel()) {
        connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, [this](const QItemSelection &selected, const QItemSelection &deselected) {
            Q_UNUSED(deselected);
            if (!selected.indexes().isEmpty()) {
                QModelIndex index = selected.indexes().first();
                qDebug() << "Selected slice item:" << index.data().toString();
            }
        });
    }
}

void SliceWidgetManager::updateContent()
{
    // Content is updated through the onSlicesParsed slot
}

void SliceWidgetManager::clearContent()
{
    if (sliceModel) {
        sliceModel->clearSliceData();
        qDebug() << "Cleared slice widget content";
    }
}

void SliceWidgetManager::connectToController(Controller *controller)
{
    // Disconnect from previous controller if any
    if (connectedController) {
        disconnect(connectedController, &Controller::slicesParsed, 
                   this, &SliceWidgetManager::onSlicesParsed);
    }
    
    connectedController = controller;
    
    if (controller) {
        connect(controller, &Controller::slicesParsed, 
                this, &SliceWidgetManager::onSlicesParsed, Qt::QueuedConnection);
        qDebug() << "Slice widget connected to controller";
    }
}

void SliceWidgetManager::onSlicesParsed(const QList<SliceInfo> &slices)
{
    if (sliceModel) {
        // Append new slices instead of replacing them
        sliceModel->appendSliceData(slices);
        
        // Expand to show the first level of items
        treeView->expandToDepth(1);
        
        qDebug() << "Appended" << slices.size() << "slices to slice widget, total:" << sliceModel->getSliceCount();
    }
} 