#include "mediafilemanager.h"
#include <QFileInfo>
#include <QDebug>

// FFmpeg headers
extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

MediaFileManager::MediaFileManager(QObject *parent)
    : QObject(parent)
    , fileSize(0)
    , formatContext(nullptr)
    , videoStream(nullptr)
    , audioStream(nullptr)
    , packet(nullptr)
    , frame(nullptr)
{
}

MediaFileManager::~MediaFileManager()
{
    closeFile();
}

bool MediaFileManager::openFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        emit error("File does not exist: " + filePath);
        return false;
    }

    currentFilePath = filePath;
    fileSize = fileInfo.size();
    
    // Add logging information
    qDebug() << "File opened successfully:";
    qDebug() << "  Name:" << fileInfo.fileName();
    qDebug() << "  Size:" << fileSize << "bytes";
    
    // Open the file with FFmpeg
    if (!openFFmpegFile(filePath)) {
        emit error("Failed to open file with FFmpeg");
        return false;
    }
    
    emit fileOpened(filePath);
    return true;
}

void MediaFileManager::closeFile()
{
    if (!currentFilePath.isEmpty()) {
        closeFFmpegFile();
        currentFilePath.clear();
        fileSize = 0;
        emit fileClosed();
    }
}

bool MediaFileManager::openFFmpegFile(const QString &filePath)
{
    // Clean up any existing resources
    cleanupFFmpegResources();

    // Open input file
    int ret = avformat_open_input(&formatContext, filePath.toUtf8().constData(), nullptr, nullptr);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, AV_ERROR_MAX_STRING_SIZE);
        emit error(QString("Could not open input file: %1").arg(errbuf));
        return false;
    }

    // Read stream information
    ret = avformat_find_stream_info(formatContext, nullptr);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, AV_ERROR_MAX_STRING_SIZE);
        emit error(QString("Could not find stream information: %1").arg(errbuf));
        return false;
    }

    // Find video and audio streams
    if (!findStreams()) {
        emit error("Could not find video or audio streams");
        return false;
    }

    // Allocate packet and frame
    packet = av_packet_alloc();
    frame = av_frame_alloc();
    if (!packet || !frame) {
        emit error("Could not allocate packet or frame");
        return false;
    }

    qDebug() << "FFmpeg file opened successfully:";
    qDebug() << "  Format:" << formatContext->iformat->name;
    qDebug() << "  Duration:" << formatContext->duration / AV_TIME_BASE << "seconds";
    qDebug() << "  Number of streams:" << formatContext->nb_streams;

    return true;
}

void MediaFileManager::closeFFmpegFile()
{
    cleanupFFmpegResources();
}

bool MediaFileManager::findStreams()
{
    videoStream = nullptr;
    audioStream = nullptr;

    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        AVStream *stream = formatContext->streams[i];
        if (!videoStream && isVideoStream(i)) {
            videoStream = stream;
            qDebug() << "Found video stream:" << i;
        }
        if (!audioStream && isAudioStream(i)) {
            audioStream = stream;
            qDebug() << "Found audio stream:" << i;
        }
    }

    return (videoStream != nullptr || audioStream != nullptr);
}

bool MediaFileManager::isVideoStream(int streamIndex) const
{
    if (!formatContext || streamIndex < 0 || streamIndex >= (int)formatContext->nb_streams) {
        return false;
    }
    return formatContext->streams[streamIndex]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO;
}

bool MediaFileManager::isAudioStream(int streamIndex) const
{
    if (!formatContext || streamIndex < 0 || streamIndex >= (int)formatContext->nb_streams) {
        return false;
    }
    return formatContext->streams[streamIndex]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO;
}

void MediaFileManager::cleanupFFmpegResources()
{
    if (formatContext) {
        avformat_close_input(&formatContext);
        formatContext = nullptr;
    }
    if (packet) {
        av_packet_free(&packet);
        packet = nullptr;
    }
    if (frame) {
        av_frame_free(&frame);
        frame = nullptr;
    }
    videoStream = nullptr;
    audioStream = nullptr;
}

QString MediaFileManager::getCurrentFilePath() const
{
    return currentFilePath;
}

qint64 MediaFileManager::getFileSize() const
{
    return fileSize;
} 