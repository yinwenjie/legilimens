#include "model/streamtreemodel.h"
#include "model/mediafilemanager.h"
#include <QStringList>

// StreamTreeItem implementation
StreamTreeItem::StreamTreeItem(const QString &name, const QString &value, StreamTreeItem *parent)
    : m_name(name), m_value(value), m_parentItem(parent)
{
    if (parent) {
        parent->appendChild(this);
    }
}

StreamTreeItem::~StreamTreeItem()
{
    qDeleteAll(m_childItems);
}

void StreamTreeItem::appendChild(StreamTreeItem *child)
{
    m_childItems.append(child);
}

StreamTreeItem *StreamTreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size()) {
        return nullptr;
    }
    return m_childItems.at(row);
}

int StreamTreeItem::childCount() const
{
    return m_childItems.count();
}

int StreamTreeItem::columnCount() const
{
    return 2; // Name and Value columns
}

QVariant StreamTreeItem::data(int column) const
{
    switch (column) {
        case 0: return m_name;
        case 1: return m_value;
        default: return QVariant();
    }
}

int StreamTreeItem::row() const
{
    if (m_parentItem) {
        return m_parentItem->m_childItems.indexOf(const_cast<StreamTreeItem*>(this));
    }
    return 0;
}

StreamTreeItem *StreamTreeItem::parentItem()
{
    return m_parentItem;
}

void StreamTreeItem::setData(const QString &name, const QVariant &value)
{
    m_name = name;
    m_value = value;
}

// StreamTreeModel implementation
StreamTreeModel::StreamTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new StreamTreeItem("Root");
    setupModelData();
}

StreamTreeModel::~StreamTreeModel()
{
    delete rootItem;
}

QVariant StreamTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    StreamTreeItem *item = static_cast<StreamTreeItem*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags StreamTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QModelIndex StreamTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    StreamTreeItem *parentItem;
    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<StreamTreeItem*>(parent.internalPointer());
    }

    StreamTreeItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex StreamTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    StreamTreeItem *childItem = static_cast<StreamTreeItem*>(index.internalPointer());
    StreamTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int StreamTreeModel::rowCount(const QModelIndex &parent) const
{
    StreamTreeItem *parentItem;
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<StreamTreeItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
}

int StreamTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2; // Property and Value columns
}

QVariant StreamTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return QString("Property");
            case 1: return QString("Value");
        }
    }
    return QVariant();
}

void StreamTreeModel::updateStreamData(const QList<VideoStreamInfo> &videoStreams, const QList<AudioStreamInfo> &audioStreams)
{
    beginResetModel();

    // Clear existing data
    delete rootItem;
    rootItem = new StreamTreeItem("Root");

    // Add video streams
    if (!videoStreams.isEmpty()) {
        StreamTreeItem *videoCategory = new StreamTreeItem("Video Streams", QString::number(videoStreams.size()), rootItem);
        
        for (int i = 0; i < videoStreams.size(); ++i) {
            const VideoStreamInfo &info = videoStreams.at(i);
            QString streamName = QString("Stream %1").arg(info.streamIndex);
            StreamTreeItem *videoStreamItem = new StreamTreeItem(streamName, info.codecId, videoCategory);
            createVideoStreamItem(info, videoStreamItem);
        }
    }

    // Add audio streams
    if (!audioStreams.isEmpty()) {
        StreamTreeItem *audioCategory = new StreamTreeItem("Audio Streams", QString::number(audioStreams.size()), rootItem);
        
        for (int i = 0; i < audioStreams.size(); ++i) {
            const AudioStreamInfo &info = audioStreams.at(i);
            QString streamName = QString("Stream %1").arg(info.streamIndex);
            StreamTreeItem *audioStreamItem = new StreamTreeItem(streamName, info.codecId, audioCategory);
            createAudioStreamItem(info, audioStreamItem);
        }
    }

    endResetModel();
}

void StreamTreeModel::clearStreamData()
{
    beginResetModel();
    delete rootItem;
    rootItem = new StreamTreeItem("Root");
    setupModelData();
    endResetModel();
}

void StreamTreeModel::setupModelData()
{
    // Create placeholder data when no file is loaded
    StreamTreeItem *placeholderItem = new StreamTreeItem("No file loaded", "", rootItem);
    Q_UNUSED(placeholderItem);
}

StreamTreeItem* StreamTreeModel::createVideoStreamItem(const VideoStreamInfo &videoInfo, StreamTreeItem *parent)
{
    addPropertyItem(parent, "Codec Type", videoInfo.codecType);
    addPropertyItem(parent, "Codec ID", videoInfo.codecId);
    addPropertyItem(parent, "Codec Tag", QString("0x%1").arg(videoInfo.codecTag, 8, 16, QChar('0')));
    addPropertyItem(parent, "Format", videoInfo.format);
    addPropertyItem(parent, "Bit Rate", QString("%1 bps").arg(videoInfo.bitRate));
    addPropertyItem(parent, "Profile", videoInfo.profile);
    addPropertyItem(parent, "Level", videoInfo.level);
    addPropertyItem(parent, "Width", QString::number(videoInfo.width));
    addPropertyItem(parent, "Height", QString::number(videoInfo.height));
    addPropertyItem(parent, "Sample Aspect Ratio", videoInfo.sampleAspectRatio);
    addPropertyItem(parent, "Field Order", videoInfo.fieldOrder);
    addPropertyItem(parent, "Color Range", videoInfo.colorRange);
    addPropertyItem(parent, "Color Primaries", videoInfo.colorPrimaries);
    addPropertyItem(parent, "Color TRC", videoInfo.colorTrc);
    addPropertyItem(parent, "Color Space", videoInfo.colorSpace);
    addPropertyItem(parent, "Chroma Location", videoInfo.chromaLocation);
    addPropertyItem(parent, "Video Delay", QString::number(videoInfo.videoDelay));

    return parent;
}

StreamTreeItem* StreamTreeModel::createAudioStreamItem(const AudioStreamInfo &audioInfo, StreamTreeItem *parent)
{
    addPropertyItem(parent, "Codec Type", audioInfo.codecType);
    addPropertyItem(parent, "Codec ID", audioInfo.codecId);
    addPropertyItem(parent, "Codec Tag", QString("0x%1").arg(audioInfo.codecTag, 8, 16, QChar('0')));
    addPropertyItem(parent, "Format", audioInfo.format);
    addPropertyItem(parent, "Bit Rate", QString("%1 bps").arg(audioInfo.bitRate));
    addPropertyItem(parent, "Profile", audioInfo.profile);
    addPropertyItem(parent, "Sample Rate", QString("%1 Hz").arg(audioInfo.sampleRate));
    addPropertyItem(parent, "Channels", QString::number(audioInfo.channels));
    addPropertyItem(parent, "Channel Layout", audioInfo.channelLayout);
    addPropertyItem(parent, "Bits Per Sample", QString::number(audioInfo.bitsPerSample));

    return parent;
}

void StreamTreeModel::addPropertyItem(StreamTreeItem *parent, const QString &name, const QVariant &value)
{
    QString valueStr = value.toString();
    if (valueStr.isEmpty() || valueStr == "unknown" || valueStr == "0") {
        valueStr = "N/A";
    }
    new StreamTreeItem(name, valueStr, parent);
} 