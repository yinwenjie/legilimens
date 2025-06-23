#include "slicetreemodel.h"
#include "mediafilemanager.h"
#include <QStringList>
#include <QDebug>

// SliceTreeItem implementation
SliceTreeItem::SliceTreeItem(const QString &name, const QString &value, SliceTreeItem *parent)
    : m_name(name), m_value(value), m_parentItem(parent)
{
    if (parent) {
        parent->appendChild(this);
    }
}

SliceTreeItem::~SliceTreeItem()
{
    qDeleteAll(m_childItems);
}

void SliceTreeItem::appendChild(SliceTreeItem *child)
{
    m_childItems.append(child);
}

SliceTreeItem *SliceTreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size()) {
        return nullptr;
    }
    return m_childItems.at(row);
}

int SliceTreeItem::childCount() const
{
    return m_childItems.count();
}

int SliceTreeItem::columnCount() const
{
    return 2; // Property and Value columns
}

QVariant SliceTreeItem::data(int column) const
{
    switch (column) {
        case 0: return m_name;
        case 1: return m_value;
        default: return QVariant();
    }
}

int SliceTreeItem::row() const
{
    if (m_parentItem) {
        return m_parentItem->m_childItems.indexOf(const_cast<SliceTreeItem*>(this));
    }
    return 0;
}

SliceTreeItem *SliceTreeItem::parentItem()
{
    return m_parentItem;
}

void SliceTreeItem::setData(const QString &name, const QVariant &value)
{
    m_name = name;
    m_value = value;
}

// SliceTreeModel implementation
SliceTreeModel::SliceTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , videoSliceCount(0)
    , audioSliceCount(0)
    , otherSliceCount(0)
{
    rootItem = new SliceTreeItem("Root");
    setupModelData();
}

SliceTreeModel::~SliceTreeModel()
{
    delete rootItem;
}

QVariant SliceTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    SliceTreeItem *item = static_cast<SliceTreeItem*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags SliceTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QModelIndex SliceTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    SliceTreeItem *parentItem;
    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<SliceTreeItem*>(parent.internalPointer());
    }

    SliceTreeItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex SliceTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    SliceTreeItem *childItem = static_cast<SliceTreeItem*>(index.internalPointer());
    SliceTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int SliceTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0) {
        return 0;
    }

    SliceTreeItem *parentItem;
    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<SliceTreeItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
}

int SliceTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2; // Property and Value columns
}

QVariant SliceTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return QString("Property");
            case 1: return QString("Value");
        }
    }
    return QVariant();
}

void SliceTreeModel::updateSliceData(const QList<SliceInfo> &slices)
{
    // Replace all existing slices with the new ones
    accumulatedSlices.clear();
    accumulatedSlices = slices;
    
    // Rebuild the tree with the new slices
    rebuildTreeFromSlices();
    
    qDebug() << "Updated slice tree model with" << slices.size() << "slices (replacing all)";
}

void SliceTreeModel::appendSliceData(const QList<SliceInfo> &slices)
{
    if (slices.isEmpty()) {
        return;
    }
    
    // Append new slices to the accumulated list
    accumulatedSlices.append(slices);
    
    // Rebuild the tree with all slices
    rebuildTreeFromSlices();
    
    qDebug() << "Appended" << slices.size() << "slices to slice tree model, total:" << accumulatedSlices.size();
}

void SliceTreeModel::clearSliceData()
{
    beginResetModel();
    delete rootItem;
    rootItem = new SliceTreeItem("Root");
    setupModelData();
    streamItems.clear();
    videoSliceCount = 0;
    audioSliceCount = 0;
    otherSliceCount = 0;
    accumulatedSlices.clear();
    endResetModel();
    
    qDebug() << "Cleared all slice data";
}

int SliceTreeModel::getSliceCount() const
{
    return accumulatedSlices.size();
}

void SliceTreeModel::rebuildTreeFromSlices()
{
    beginResetModel();
    
    // Clear existing tree structure
    delete rootItem;
    rootItem = new SliceTreeItem("Root");
    streamItems.clear();
    
    // Reset counters
    videoSliceCount = 0;
    audioSliceCount = 0;
    otherSliceCount = 0;
    
    // Create category items
    SliceTreeItem *videoCategory = new SliceTreeItem("Video Slices", "0", rootItem);
    SliceTreeItem *audioCategory = new SliceTreeItem("Audio Slices", "0", rootItem);
    SliceTreeItem *otherCategory = new SliceTreeItem("Other Slices", "0", rootItem);
    
    // Process all accumulated slices
    for (const SliceInfo &slice : accumulatedSlices) {
        // Get or create stream item
        SliceTreeItem *streamParent;
        if (slice.streamType == "video") {
            streamParent = videoCategory;
            videoSliceCount++;
        } else if (slice.streamType == "audio") {
            streamParent = audioCategory;
            audioSliceCount++;
        } else {
            streamParent = otherCategory;
            otherSliceCount++;
        }
        
        // Get or create stream-specific container
        int streamIndex = slice.streamIndex;
        if (!streamItems.contains(streamIndex)) {
            QString streamName = QString("Stream %1").arg(streamIndex);
            streamItems[streamIndex] = new SliceTreeItem(streamName, slice.streamType, streamParent);
        }
        
        // Create slice item
        QString sliceName = QString("Slice at PTS %1").arg(formatTimestamp(slice.pts));
        SliceTreeItem *sliceItem = new SliceTreeItem(sliceName, 
                                                    slice.isKeyFrame ? "Key Frame" : "Regular Frame",
                                                    streamItems[streamIndex]);
        
        // Add slice details
        createSliceItem(slice, sliceItem);
    }
    
    // Update category counts
    videoCategory->setData("Video Slices", QString::number(videoSliceCount));
    audioCategory->setData("Audio Slices", QString::number(audioSliceCount));
    otherCategory->setData("Other Slices", QString::number(otherSliceCount));
    
    // Remove empty categories
    if (videoSliceCount == 0) {
        rootItem->child(0)->setData("Video Slices", "None");
    }
    if (audioSliceCount == 0) {
        rootItem->child(1)->setData("Audio Slices", "None");
    }
    if (otherSliceCount == 0) {
        rootItem->child(2)->setData("Other Slices", "None");
    }
    
    endResetModel();
    
    qDebug() << "Rebuilt slice tree model with" << accumulatedSlices.size() << "total slices";
    qDebug() << "  Video slices:" << videoSliceCount;
    qDebug() << "  Audio slices:" << audioSliceCount;
    qDebug() << "  Other slices:" << otherSliceCount;
}

void SliceTreeModel::setupModelData()
{
    // Create placeholder data when no file is loaded
    SliceTreeItem *placeholderItem = new SliceTreeItem("No slices available", "", rootItem);
    Q_UNUSED(placeholderItem);
}

SliceTreeItem* SliceTreeModel::createSliceItem(const SliceInfo &sliceInfo, SliceTreeItem *parent)
{
    addPropertyItem(parent, "Stream Index", QString::number(sliceInfo.streamIndex));
    addPropertyItem(parent, "Stream Type", sliceInfo.streamType);
    addPropertyItem(parent, "PTS", formatTimestamp(sliceInfo.pts));
    addPropertyItem(parent, "DTS", formatTimestamp(sliceInfo.dts));
    addPropertyItem(parent, "Duration", QString::number(sliceInfo.duration));
    addPropertyItem(parent, "Size", QString("%1 bytes").arg(sliceInfo.size));
    addPropertyItem(parent, "Key Frame", sliceInfo.isKeyFrame ? "Yes" : "No");
    addPropertyItem(parent, "Position", QString("0x%1").arg(sliceInfo.pos, 0, 16));
    
    return parent;
}

void SliceTreeModel::addPropertyItem(SliceTreeItem *parent, const QString &name, const QVariant &value)
{
    QString valueStr = value.toString();
    if (valueStr.isEmpty()) {
        valueStr = "N/A";
    }
    new SliceTreeItem(name, valueStr, parent);
}

QString SliceTreeModel::formatTimestamp(int64_t timestamp) const
{
    // Format timestamp in a more readable way
    if (timestamp < 0) {
        return "N/A";
    }
    return QString("%1").arg(timestamp);
} 