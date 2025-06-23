#include "view/widgets/slicewidgetmanager.h"
#include "model/slicetreemodel.h"
#include "model/sliceprocessor.h"
#include "controller/controller.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

SliceWidgetManager::SliceWidgetManager(QWidget *parent)
    : BaseWidgetManager(parent)
    , treeView(nullptr)
    , sliceModel(nullptr)
    , connectedController(nullptr)
    , sliceProcessor(nullptr)
{
    // Create the slice processor and move it to a separate thread
    sliceProcessor = new SliceProcessor();
    sliceProcessor->moveToThread(&processorThread);
    
    // Connect thread start signal to processor's process slot
    connect(&processorThread, &QThread::started, sliceProcessor, &SliceProcessor::process);
    
    // Connect processor signals to our slots
    connect(sliceProcessor, &SliceProcessor::slicesBatchProcessed, 
            this, &SliceWidgetManager::onSlicesBatchProcessed, Qt::QueuedConnection);
    connect(sliceProcessor, &SliceProcessor::processingFinished,
            this, &SliceWidgetManager::onSliceProcessingFinished, Qt::QueuedConnection);
    
    // Start the processor thread
    processorThread.start();
    sliceProcessor->start();
}

SliceWidgetManager::~SliceWidgetManager()
{
    // Stop the processor and wait for the thread to finish
    if (sliceProcessor) {
        sliceProcessor->stop();
    }
    
    processorThread.quit();
    processorThread.wait();
    
    // Delete the processor (it's safe now that the thread has stopped)
    delete sliceProcessor;
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
    }
    
    if (sliceProcessor) {
        sliceProcessor->clearSlices();
    }
    
    qDebug() << "Cleared slice widget content";
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
    if (slices.isEmpty()) {
        return;
    }
    
    // Queue the slices for background processing
    if (sliceProcessor) {
        sliceProcessor->queueSlices(slices);
    }
    
    qDebug() << "Queued" << slices.size() << "slices for background processing";
}

void SliceWidgetManager::onSlicesBatchProcessed(const QList<SliceInfo> &slices)
{
    if (sliceModel && !slices.isEmpty()) {
        // Append the processed batch to the model
        sliceModel->appendSliceData(slices);
        
        // Expand to show the first level of items
        treeView->expandToDepth(1);
        
        qDebug() << "Added batch of" << slices.size() << "slices to tree model, total:" << sliceModel->getSliceCount();
    }
}

void SliceWidgetManager::onSliceProcessingFinished()
{
    qDebug() << "Slice processing finished, total slices in model:" << sliceModel->getSliceCount();
} 