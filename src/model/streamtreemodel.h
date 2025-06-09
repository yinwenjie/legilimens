#ifndef STREAMTREEMODEL_H
#define STREAMTREEMODEL_H

#include <QAbstractItemModel>
#include <QString>
#include <QVector>
#include <QVariant>

// Forward declarations
struct VideoStreamInfo;
struct AudioStreamInfo;

class StreamTreeItem
{
public:
    explicit StreamTreeItem(const QString &name, const QString &value = QString(), StreamTreeItem *parent = nullptr);
    ~StreamTreeItem();

    void appendChild(StreamTreeItem *child);
    StreamTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    StreamTreeItem *parentItem();

    // Set data
    void setData(const QString &name, const QVariant &value);

private:
    QVector<StreamTreeItem*> m_childItems;
    QString m_name;
    QVariant m_value;
    StreamTreeItem *m_parentItem;
};

class StreamTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit StreamTreeModel(QObject *parent = nullptr);
    ~StreamTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Stream data management
    void updateStreamData(const QList<VideoStreamInfo> &videoStreams, const QList<AudioStreamInfo> &audioStreams);
    void clearStreamData();

private:
    StreamTreeItem *rootItem;

    // Helper methods
    void setupModelData();
    StreamTreeItem* createVideoStreamItem(const VideoStreamInfo &videoInfo, StreamTreeItem *parent);
    StreamTreeItem* createAudioStreamItem(const AudioStreamInfo &audioInfo, StreamTreeItem *parent);
    void addPropertyItem(StreamTreeItem *parent, const QString &name, const QVariant &value);
};

#endif // STREAMTREEMODEL_H 