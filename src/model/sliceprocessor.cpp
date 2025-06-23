#include "sliceprocessor.h"
#include "mediafilemanager.h"
#include <QDebug>
#include <QThread>

SliceProcessor::SliceProcessor(QObject *parent)
    : QObject(parent)
    , running(false)
    , batchSize(50) // Default batch size
{
    qDebug() << "SliceProcessor created";
}

SliceProcessor::~SliceProcessor()
{
    stop();
    qDebug() << "SliceProcessor destroyed";
}

void SliceProcessor::start()
{
    QMutexLocker locker(&mutex);
    running = true;
    qDebug() << "SliceProcessor started";
}

void SliceProcessor::stop()
{
    QMutexLocker locker(&mutex);
    running = false;
    condition.wakeAll(); // Wake up any waiting threads
    qDebug() << "SliceProcessor stopped";
}

void SliceProcessor::queueSlices(const QList<SliceInfo> &slices)
{
    if (slices.isEmpty()) {
        return;
    }
    
    QMutexLocker locker(&mutex);
    
    // Add all slices to the queue
    for (const SliceInfo &slice : slices) {
        sliceQueue.enqueue(slice);
    }
    
    qDebug() << "Added" << slices.size() << "slices to processing queue, total:" << sliceQueue.size();
    
    // Signal that there are slices to process
    condition.wakeOne();
}

void SliceProcessor::clearSlices()
{
    QMutexLocker locker(&mutex);
    sliceQueue.clear();
    processedSlices.clear();
    qDebug() << "Cleared all slices from processor";
}

bool SliceProcessor::isRunning() const
{
    return running;
}

void SliceProcessor::setBatchSize(int size)
{
    if (size > 0) {
        QMutexLocker locker(&mutex);
        batchSize = size;
        qDebug() << "Slice batch size set to" << batchSize;
    }
}

void SliceProcessor::process()
{
    qDebug() << "SliceProcessor process thread started";
    
    while (true) {
        QList<SliceInfo> batch;
        
        {
            QMutexLocker locker(&mutex);
            
            // Wait until there are slices to process or we're told to stop
            while (sliceQueue.isEmpty() && running) {
                condition.wait(&mutex);
            }
            
            // Check if we should exit
            if (!running && sliceQueue.isEmpty()) {
                break;
            }
            
            // Process a batch of slices
            int count = qMin(batchSize, sliceQueue.size());
            for (int i = 0; i < count; ++i) {
                if (!sliceQueue.isEmpty()) {
                    SliceInfo slice = sliceQueue.dequeue();
                    batch.append(slice);
                    processedSlices.append(slice);
                }
            }
        }
        
        if (!batch.isEmpty()) {
            // Emit signal with the batch of processed slices
            emit slicesBatchProcessed(batch);
            
            qDebug() << "Processed batch of" << batch.size() << "slices, remaining:" << sliceQueue.size();
            
            // Small delay to allow UI to update
            QThread::msleep(10);
        }
        
        // Check if we've processed all slices
        if (sliceQueue.isEmpty()) {
            emit processingFinished();
        }
    }
    
    qDebug() << "SliceProcessor process thread finished";
} 