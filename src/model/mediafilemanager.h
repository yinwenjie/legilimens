#ifndef MEDIAFILEMANAGER_H
#define MEDIAFILEMANAGER_H

#include <QObject>
#include <QString>
#include <QFileInfo>

// Forward declarations for FFmpeg structures
struct AVFormatContext;
struct AVStream;
struct AVPacket;
struct AVFrame;

class MediaFileManager : public QObject
{
    Q_OBJECT

public:
    explicit MediaFileManager(QObject *parent = nullptr);
    virtual ~MediaFileManager();

    // File operations
    bool openFile(const QString &filePath);
    void closeFile();
    QString getCurrentFilePath() const;
    qint64 getFileSize() const;

    // FFmpeg operations
    bool openFFmpegFile(const QString &filePath);
    void closeFFmpegFile();
    bool findStreams();
    bool isVideoStream(int streamIndex) const;
    bool isAudioStream(int streamIndex) const;
    AVStream* getVideoStream() const { return videoStream; }
    AVStream* getAudioStream() const { return audioStream; }
    AVFormatContext* getFormatContext() const { return formatContext; }

signals:
    void fileOpened(const QString &filePath);
    void fileClosed();
    void error(const QString &message);

private:
    QString currentFilePath;
    qint64 fileSize;

    // FFmpeg context pointers
    AVFormatContext *formatContext;
    AVStream *videoStream;
    AVStream *audioStream;
    AVPacket *packet;
    AVFrame *frame;

    // Helper methods
    void cleanupFFmpegResources();
};

#endif // MEDIAFILEMANAGER_H 