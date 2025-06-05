#include "model/streamtreemodel.h"

// StreamTreeItem implementation
StreamTreeItem::StreamTreeItem(const QString &name, StreamTreeItem *parent)
    : m_name(name)
    , m_parentItem(parent)
{
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
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int StreamTreeItem::childCount() const
{
    return m_childItems.count();
}

int StreamTreeItem::columnCount() const
{
    return 1;  // We only have one column for now
}

QString StreamTreeItem::data(int column) const
{
    if (column == 0)
        return m_name;
    return QString();
}

int StreamTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<StreamTreeItem*>(this));
    return 0;
}

StreamTreeItem *StreamTreeItem::parentItem()
{
    return m_parentItem;
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
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    StreamTreeItem *item = static_cast<StreamTreeItem*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags StreamTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QModelIndex StreamTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    StreamTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<StreamTreeItem*>(parent.internalPointer());

    StreamTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex StreamTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    StreamTreeItem *childItem = static_cast<StreamTreeItem*>(index.internalPointer());
    StreamTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int StreamTreeModel::rowCount(const QModelIndex &parent) const
{
    StreamTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<StreamTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int StreamTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;  // We only have one column for now
}

QVariant StreamTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0)
            return tr("Stream Information");
    }
    return QVariant();
}

void StreamTreeModel::setupModelData()
{
    // Example data structure - this will be replaced with actual stream data
    StreamTreeItem *videoStream = new StreamTreeItem("Video Stream", rootItem);
    videoStream->appendChild(new StreamTreeItem("Codec: H.264", videoStream));
    videoStream->appendChild(new StreamTreeItem("Resolution: 1920x1080", videoStream));
    videoStream->appendChild(new StreamTreeItem("Frame Rate: 30 fps", videoStream));
    rootItem->appendChild(videoStream);

    StreamTreeItem *audioStream = new StreamTreeItem("Audio Stream", rootItem);
    audioStream->appendChild(new StreamTreeItem("Codec: AAC", audioStream));
    audioStream->appendChild(new StreamTreeItem("Sample Rate: 48 kHz", audioStream));
    audioStream->appendChild(new StreamTreeItem("Channels: 2", audioStream));
    rootItem->appendChild(audioStream);
} 