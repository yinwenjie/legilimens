#ifndef MEDIAPARSERTHREAD_H
#define MEDIAPARSERTHREAD_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QString>

// Forward declarations
struct AVFormatContext;
struct AVPacket;
struct SliceInfo;

class MediaParserThread : public QObject
{
    Q_OBJECT

public:
    explicit MediaParserThread(QObject *parent = nullptr);
    ~MediaParserThread();

    // Set the file to parse
    void setFilePath(const QString &filePath);
    
    // Control parsing
    void requestStop();
    bool isStopped() const;

public slots:
    void startParsing();

signals:
    void slicesParsed(const QList<SliceInfo> &slices);
    void parsingProgress(int percentage);
    void parsingFinished();
    void error(const QString &message);

private:
    QString filePath;
    mutable QMutex mutex;
    bool stopRequested;
    
    // FFmpeg context for parsing
    AVFormatContext *parseContext;
    AVPacket *parsePacket;
    
    // Helper methods
    bool openFile();
    void closeFile();
    SliceInfo createSliceInfo(AVPacket *packet, int streamIndex) const;
    QString getStreamType(int streamIndex) const;
    void cleanupResources();
};

#endif // MEDIAPARSERTHREAD_H 