#ifndef SLICEPROCESSOR_H
#define SLICEPROCESSOR_H

#include <QObject>
#include <QList>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

// Forward declarations
struct SliceInfo;

/**
 * @brief The SliceProcessor class handles slice processing in a background thread
 * 
 * This class processes slices in a separate thread to keep the UI responsive.
 * It batches slices and emits signals when batches are ready for the UI to display.
 */
class SliceProcessor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Slice Processor
     * @param parent The parent QObject
     */
    explicit SliceProcessor(QObject *parent = nullptr);
    
    /**
     * @brief Destroy the Slice Processor
     */
    ~SliceProcessor();
    
    /**
     * @brief Start the processor thread
     */
    void start();
    
    /**
     * @brief Stop the processor thread
     */
    void stop();
    
    /**
     * @brief Queue slices for processing
     * @param slices The slices to process
     */
    void queueSlices(const QList<SliceInfo> &slices);
    
    /**
     * @brief Clear all queued and processed slices
     */
    void clearSlices();
    
    /**
     * @brief Check if the processor is running
     * @return bool True if running
     */
    bool isRunning() const;
    
    /**
     * @brief Set the batch size for processing
     * @param size The number of slices to process in each batch
     */
    void setBatchSize(int size);

signals:
    /**
     * @brief Signal emitted when a batch of slices is processed
     * @param slices The processed slice batch
     */
    void slicesBatchProcessed(const QList<SliceInfo> &slices);
    
    /**
     * @brief Signal emitted when all queued slices are processed
     */
    void processingFinished();

public slots:
    /**
     * @brief Process slices in the queue
     */
    void process();

private:
    QMutex mutex;                  ///< Mutex for thread safety
    QWaitCondition condition;      ///< Wait condition for thread synchronization
    QQueue<SliceInfo> sliceQueue;  ///< Queue of slices to process
    QList<SliceInfo> processedSlices; ///< List of already processed slices
    bool running;                  ///< Running flag
    int batchSize;                 ///< Number of slices to process in each batch
};

#endif // SLICEPROCESSOR_H 