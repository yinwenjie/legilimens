#include "mediaparserthread.h"
#include "mediafilemanager.h"
#include <QMutexLocker>
#include <QDebug>
#include <QThread>

// FFmpeg headers
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/timestamp.h>
}

MediaParserThread::MediaParserThread(QObject *parent)
    : QObject(parent)
    , stopRequested(false)
    , parseContext(nullptr)
    , parsePacket(nullptr)
{
    parsePacket = av_packet_alloc();
}

MediaParserThread::~MediaParserThread()
{
    cleanupResources();
}

void MediaParserThread::setFilePath(const QString &filePath)
{
    QMutexLocker locker(&mutex);
    this->filePath = filePath;
}

void MediaParserThread::requestStop()
{
    QMutexLocker locker(&mutex);
    stopRequested = true;
}

bool MediaParserThread::isStopped() const
{
    QMutexLocker locker(&mutex);
    return stopRequested;
}

void MediaParserThread::startParsing()
{
    {
        QMutexLocker locker(&mutex);
        stopRequested = false;
    }
    
    qDebug() << "Starting media file parsing in thread:" << QThread::currentThread();
    
    if (!openFile()) {
        emit error("Failed to open file for parsing");
        return;
    }
    
    QList<SliceInfo> slices;
    int64_t totalDuration = parseContext->duration;
    int64_t currentTime = 0;
    int lastProgress = -1;
    int sliceCount = 0;
    
    qDebug() << "Starting packet parsing. Total duration:" << totalDuration;
    
    // Parse packets from the file
    while (av_read_frame(parseContext, parsePacket) >= 0) {
        // Check if stop was requested
        if (isStopped()) {
            qDebug() << "Parsing stopped by request";
            break;
        }
        
        // Create slice info from packet
        SliceInfo slice = createSliceInfo(parsePacket, parsePacket->stream_index);
        slices.append(slice);
        sliceCount++;
        
        // Log detailed information for first 10 slices, then every 50th slice
        if (sliceCount <= 10 || sliceCount % 50 == 0) {
            QString logMsg = QString("Slice #%1: Stream %2 (%3), PTS: %4, DTS: %5, Duration: %6, Size: %7 bytes, KeyFrame: %8, Pos: %9")
                        .arg(sliceCount)
                        .arg(slice.streamIndex)
                        .arg(slice.streamType)
                        .arg(slice.pts)
                        .arg(slice.dts)
                        .arg(slice.duration)
                        .arg(slice.size)
                        .arg(slice.isKeyFrame ? "Yes" : "No")
                        .arg(slice.pos);
            
            if (sliceCount <= 10) {
                qDebug() << "[FIRST_SLICES]" << logMsg;
            } else {
                qDebug() << "[PROGRESS]" << logMsg;
            }
        }
        
        // Update progress
        if (totalDuration > 0) {
            currentTime = parsePacket->pts;
            if (currentTime > 0) {
                int progress = static_cast<int>((currentTime * 100) / totalDuration);
                if (progress != lastProgress && progress >= 0 && progress <= 100) {
                    emit parsingProgress(progress);
                    lastProgress = progress;
                }
            }
        }
        
        // Emit slices in batches to avoid overwhelming the UI
        if (slices.size() >= 100) {
            qDebug() << QString("Emitting batch of %1 slices (total processed: %2)")
                        .arg(slices.size())
                        .arg(sliceCount);
            emit slicesParsed(slices);
            slices.clear();
        }
        
        // Unref the packet
        av_packet_unref(parsePacket);
    }
    
    // Emit any remaining slices
    if (!slices.isEmpty()) {
        qDebug() << QString("Emitting final batch of %1 slices").arg(slices.size());
        emit slicesParsed(slices);
    }
    
    // Log parsing summary
    qDebug() << "=== PARSING SUMMARY ===";
    qDebug() << QString("Total slices processed: %1").arg(sliceCount);
    qDebug() << QString("File duration: %1 seconds").arg(totalDuration / AV_TIME_BASE);
    qDebug() << QString("Number of streams: %1").arg(parseContext->nb_streams);
    
    // Count slices by stream type
    int videoSlices = 0, audioSlices = 0, otherSlices = 0;
    for (int i = 0; i < parseContext->nb_streams; i++) {
        QString streamType = getStreamType(i);
        if (streamType == "video") videoSlices++;
        else if (streamType == "audio") audioSlices++;
        else otherSlices++;
    }
    qDebug() << QString("Streams breakdown - Video: %1, Audio: %2, Other: %3")
                .arg(videoSlices).arg(audioSlices).arg(otherSlices);
    qDebug() << "=======================";
    
    closeFile();
    
    if (!isStopped()) {
        emit parsingProgress(100);
        emit parsingFinished();
        qDebug() << "Media file parsing completed successfully";
    }
}

bool MediaParserThread::openFile()
{
    QString currentFilePath;
    {
        QMutexLocker locker(&mutex);
        currentFilePath = filePath;
    }
    
    if (currentFilePath.isEmpty()) {
        return false;
    }
    
    // Open input file
    int ret = avformat_open_input(&parseContext, currentFilePath.toUtf8().constData(), nullptr, nullptr);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, AV_ERROR_MAX_STRING_SIZE);
        emit error(QString("Could not open input file for parsing: %1").arg(errbuf));
        return false;
    }
    
    // Read stream information
    ret = avformat_find_stream_info(parseContext, nullptr);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, AV_ERROR_MAX_STRING_SIZE);
        emit error(QString("Could not find stream information for parsing: %1").arg(errbuf));
        return false;
    }
    
    qDebug() << "Parser opened file successfully:" << currentFilePath;
    
    // Log file context information
    qDebug() << "=== FILE CONTEXT INFO ===";
    qDebug() << QString("Format: %1").arg(parseContext->iformat->name);
    qDebug() << QString("Duration: %1 seconds").arg(parseContext->duration / AV_TIME_BASE);
    qDebug() << QString("Bit rate: %1 bps").arg(parseContext->bit_rate);
    qDebug() << QString("Number of streams: %1").arg(parseContext->nb_streams);
    
    // Log information about each stream
    for (unsigned int i = 0; i < parseContext->nb_streams; i++) {
        AVStream *stream = parseContext->streams[i];
        AVCodecParameters *codecpar = stream->codecpar;
        QString streamType = getStreamType(i);
        
        qDebug() << QString("Stream %1: Type=%2, Codec=%3, Bitrate=%4")
                    .arg(i)
                    .arg(streamType)
                    .arg(avcodec_get_name(codecpar->codec_id))
                    .arg(codecpar->bit_rate);
        
        if (streamType == "video") {
            qDebug() << QString("  Video: %1x%2, FPS: %3")
                        .arg(codecpar->width)
                        .arg(codecpar->height)
                        .arg(av_q2d(stream->avg_frame_rate));
        } else if (streamType == "audio") {
            qDebug() << QString("  Audio: %1 Hz, %2 channels")
                        .arg(codecpar->sample_rate)
                        .arg(codecpar->ch_layout.nb_channels);
        }
    }
    qDebug() << "========================";
    
    return true;
}

void MediaParserThread::closeFile()
{
    if (parseContext) {
        avformat_close_input(&parseContext);
        parseContext = nullptr;
    }
}

SliceInfo MediaParserThread::createSliceInfo(AVPacket *packet, int streamIndex) const
{
    SliceInfo slice;
    
    slice.streamIndex = streamIndex;
    slice.pts = packet->pts;
    slice.dts = packet->dts;
    slice.duration = packet->duration;
    slice.pos = packet->pos;
    slice.size = packet->size;
    slice.isKeyFrame = (packet->flags & AV_PKT_FLAG_KEY) != 0;
    slice.streamType = getStreamType(streamIndex);
    
    return slice;
}

QString MediaParserThread::getStreamType(int streamIndex) const
{
    if (!parseContext || streamIndex < 0 || streamIndex >= (int)parseContext->nb_streams) {
        return "unknown";
    }
    
    AVCodecParameters *codecpar = parseContext->streams[streamIndex]->codecpar;
    switch (codecpar->codec_type) {
        case AVMEDIA_TYPE_VIDEO: return "video";
        case AVMEDIA_TYPE_AUDIO: return "audio";
        case AVMEDIA_TYPE_SUBTITLE: return "subtitle";
        case AVMEDIA_TYPE_DATA: return "data";
        default: return "unknown";
    }
}

void MediaParserThread::cleanupResources()
{
    closeFile();
    if (parsePacket) {
        av_packet_free(&parsePacket);
        parsePacket = nullptr;
    }
} 