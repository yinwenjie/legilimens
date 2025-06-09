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
    // Register meta types for signal-slot system
    qRegisterMetaType<VideoStreamInfo>("VideoStreamInfo");
    qRegisterMetaType<AudioStreamInfo>("AudioStreamInfo");
    qRegisterMetaType<QList<VideoStreamInfo>>("QList<VideoStreamInfo>");
    qRegisterMetaType<QList<AudioStreamInfo>>("QList<AudioStreamInfo>");
}

MediaFileManager::~MediaFileManager()
{
    // Disconnect all signals to prevent issues during destruction
    disconnect();
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

    // Extract all stream information
    extractAllStreamInfo();

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

    // Clear stream information lists
    videoStreamInfoList.clear();
    audioStreamInfoList.clear();
}

QString MediaFileManager::getCurrentFilePath() const
{
    return currentFilePath;
}

qint64 MediaFileManager::getFileSize() const
{
    return fileSize;
}

void MediaFileManager::extractAllStreamInfo()
{
    videoStreamInfoList.clear();
    audioStreamInfoList.clear();

    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (isVideoStream(i)) {
            VideoStreamInfo info = extractVideoStreamInfo(i);
            videoStreamInfoList.append(info);
        } else if (isAudioStream(i)) {
            AudioStreamInfo info = extractAudioStreamInfo(i);
            audioStreamInfoList.append(info);
        }
    }

    emit streamsInfoUpdated(videoStreamInfoList, audioStreamInfoList);
    qDebug() << "Extracted" << videoStreamInfoList.size() << "video streams and" << audioStreamInfoList.size() << "audio streams";
}

VideoStreamInfo MediaFileManager::extractVideoStreamInfo(int streamIndex) const
{
    VideoStreamInfo info;

    if (!formatContext || streamIndex < 0 || streamIndex >= (int)formatContext->nb_streams) {
        return info;
    }

    AVStream *stream = formatContext->streams[streamIndex];
    AVCodecParameters *codecpar = stream->codecpar;

    info.streamIndex = streamIndex;
    info.codecType = "video";
    info.codecId = getCodecName(codecpar->codec_id);
    info.codecTag = codecpar->codec_tag;
    info.format = getPixelFormatName(codecpar->format);
    info.bitRate = codecpar->bit_rate;
    info.profile = getProfileName(codecpar->codec_id, codecpar->profile);
    info.level = getLevelName(codecpar->codec_id, codecpar->level);
    info.width = codecpar->width;
    info.height = codecpar->height;
    info.sampleAspectRatio = formatAspectRatio(codecpar->sample_aspect_ratio.num, codecpar->sample_aspect_ratio.den);
    info.fieldOrder = getFieldOrderName(codecpar->field_order);
    info.colorRange = getColorRangeName(codecpar->color_range);
    info.colorPrimaries = getColorPrimariesName(codecpar->color_primaries);
    info.colorTrc = getColorTrcName(codecpar->color_trc);
    info.colorSpace = getColorSpaceName(codecpar->color_space);
    info.chromaLocation = getChromaLocationName(codecpar->chroma_location);
    info.videoDelay = stream->codecpar->video_delay;

    return info;
}

AudioStreamInfo MediaFileManager::extractAudioStreamInfo(int streamIndex) const
{
    AudioStreamInfo info;

    if (!formatContext || streamIndex < 0 || streamIndex >= (int)formatContext->nb_streams) {
        return info;
    }

    AVStream *stream = formatContext->streams[streamIndex];
    AVCodecParameters *codecpar = stream->codecpar;

    info.streamIndex = streamIndex;
    info.codecType = "audio";
    info.codecId = getCodecName(codecpar->codec_id);
    info.codecTag = codecpar->codec_tag;
    info.format = av_get_sample_fmt_name(static_cast<AVSampleFormat>(codecpar->format));
    info.bitRate = codecpar->bit_rate;
    info.profile = getProfileName(codecpar->codec_id, codecpar->profile);
    info.sampleRate = codecpar->sample_rate;
    info.channels = codecpar->ch_layout.nb_channels;

    char layout_name[256];
    av_channel_layout_describe(&codecpar->ch_layout, layout_name, sizeof(layout_name));
    info.channelLayout = QString(layout_name);

    info.bitsPerSample = av_get_bits_per_sample(codecpar->codec_id);

    return info;
}

QList<VideoStreamInfo> MediaFileManager::getVideoStreamInfoList() const
{
    return videoStreamInfoList;
}

QList<AudioStreamInfo> MediaFileManager::getAudioStreamInfoList() const
{
    return audioStreamInfoList;
}

int MediaFileManager::getVideoStreamCount() const
{
    return videoStreamInfoList.size();
}

int MediaFileManager::getAudioStreamCount() const
{
    return audioStreamInfoList.size();
}

int MediaFileManager::getTotalStreamCount() const
{
    return formatContext ? formatContext->nb_streams : 0;
}

QString MediaFileManager::getCodecName(int codecId) const
{
    const AVCodec *codec = avcodec_find_decoder(static_cast<AVCodecID>(codecId));
    if (codec && codec->name) {
        return QString(codec->name);
    }
    return QString("unknown");
}

QString MediaFileManager::getPixelFormatName(int pixFmt) const
{
    const char *name = av_get_pix_fmt_name(static_cast<AVPixelFormat>(pixFmt));
    return name ? QString(name) : QString("unknown");
}

QString MediaFileManager::getColorRangeName(int colorRange) const
{
    switch (colorRange) {
        case AVCOL_RANGE_MPEG: return "tv";
        case AVCOL_RANGE_JPEG: return "pc";
        default: return "unknown";
    }
}

QString MediaFileManager::getColorPrimariesName(int colorPrimaries) const
{
    const char *name = av_color_primaries_name(static_cast<AVColorPrimaries>(colorPrimaries));
    return name ? QString(name) : QString("unknown");
}

QString MediaFileManager::getColorTrcName(int colorTrc) const
{
    const char *name = av_color_transfer_name(static_cast<AVColorTransferCharacteristic>(colorTrc));
    return name ? QString(name) : QString("unknown");
}

QString MediaFileManager::getColorSpaceName(int colorSpace) const
{
    const char *name = av_color_space_name(static_cast<AVColorSpace>(colorSpace));
    return name ? QString(name) : QString("unknown");
}

QString MediaFileManager::getChromaLocationName(int chromaLocation) const
{
    const char *name = av_chroma_location_name(static_cast<AVChromaLocation>(chromaLocation));
    return name ? QString(name) : QString("unknown");
}

QString MediaFileManager::getFieldOrderName(int fieldOrder) const
{
    switch (fieldOrder) {
        case AV_FIELD_PROGRESSIVE: return "progressive";
        case AV_FIELD_TT: return "tt";
        case AV_FIELD_BB: return "bb";
        case AV_FIELD_TB: return "tb";
        case AV_FIELD_BT: return "bt";
        default: return "unknown";
    }
}

QString MediaFileManager::getProfileName(int codecId, int profile) const
{
    const char *name = avcodec_profile_name(static_cast<AVCodecID>(codecId), profile);
    return name ? QString(name) : QString("unknown");
}

QString MediaFileManager::getLevelName(int codecId, int level) const
{
    if (level == AV_LEVEL_UNKNOWN) {
        return QString("unknown");
    }
    return QString::number(level);
}

QString MediaFileManager::formatAspectRatio(int num, int den) const
{
    if (den == 0) {
        return QString("unknown");
    }
    return QString("%1:%2").arg(num).arg(den);
} 