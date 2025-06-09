#ifndef MEDIAFILEMANAGER_H
#define MEDIAFILEMANAGER_H

#include <QObject>
#include <QString>
#include <QFileInfo>
#include <QList>

// Forward declarations for FFmpeg structures
struct AVFormatContext;
struct AVStream;
struct AVPacket;
struct AVFrame;

// Video stream information structure
struct VideoStreamInfo {
    int streamIndex;
    QString codecType;
    QString codecId;
    uint32_t codecTag;
    QString format;
    int64_t bitRate;
    QString profile;
    QString level;
    int width;
    int height;
    QString sampleAspectRatio;
    QString fieldOrder;
    QString colorRange;
    QString colorPrimaries;
    QString colorTrc;
    QString colorSpace;
    QString chromaLocation;
    int videoDelay;

    // Constructor
    VideoStreamInfo() : streamIndex(-1), codecTag(0), bitRate(0), width(0), height(0), videoDelay(0) {}
};

// Audio stream information structure
struct AudioStreamInfo {
    int streamIndex;
    QString codecType;
    QString codecId;
    uint32_t codecTag;
    QString format;
    int64_t bitRate;
    QString profile;
    int sampleRate;
    int channels;
    QString channelLayout;
    int bitsPerSample;

    // Constructor
    AudioStreamInfo() : streamIndex(-1), codecTag(0), bitRate(0), sampleRate(0), channels(0), bitsPerSample(0) {}
};

// Register types with Qt's meta-object system
Q_DECLARE_METATYPE(VideoStreamInfo)
Q_DECLARE_METATYPE(AudioStreamInfo)
Q_DECLARE_METATYPE(QList<VideoStreamInfo>)
Q_DECLARE_METATYPE(QList<AudioStreamInfo>)

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

    // Stream information extraction
    QList<VideoStreamInfo> getVideoStreamInfoList() const;
    QList<AudioStreamInfo> getAudioStreamInfoList() const;
    VideoStreamInfo extractVideoStreamInfo(int streamIndex) const;
    AudioStreamInfo extractAudioStreamInfo(int streamIndex) const;

    // Stream count getters
    int getVideoStreamCount() const;
    int getAudioStreamCount() const;
    int getTotalStreamCount() const;

signals:
    void fileOpened(const QString &filePath);
    void fileClosed();
    void error(const QString &message);
    void streamsInfoUpdated(const QList<VideoStreamInfo> &videoStreams, const QList<AudioStreamInfo> &audioStreams);

private:
    QString currentFilePath;
    qint64 fileSize;

    // FFmpeg context pointers
    AVFormatContext *formatContext;
    AVStream *videoStream;
    AVStream *audioStream;
    AVPacket *packet;
    AVFrame *frame;

    // Stream information lists
    QList<VideoStreamInfo> videoStreamInfoList;
    QList<AudioStreamInfo> audioStreamInfoList;

    // Helper methods
    void cleanupFFmpegResources();
    void extractAllStreamInfo();
    QString getCodecName(int codecId) const;
    QString getPixelFormatName(int pixFmt) const;
    QString getColorRangeName(int colorRange) const;
    QString getColorPrimariesName(int colorPrimaries) const;
    QString getColorTrcName(int colorTrc) const;
    QString getColorSpaceName(int colorSpace) const;
    QString getChromaLocationName(int chromaLocation) const;
    QString getFieldOrderName(int fieldOrder) const;
    QString getProfileName(int codecId, int profile) const;
    QString getLevelName(int codecId, int level) const;
    QString formatAspectRatio(int num, int den) const;
};

#endif // MEDIAFILEMANAGER_H 