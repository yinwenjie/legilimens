#ifndef STREAMTREEMODEL_H
#define STREAMTREEMODEL_H

#include <QAbstractItemModel>
#include <QString>
#include <QVector>

class StreamTreeItem
{
public:
    explicit StreamTreeItem(const QString &name, StreamTreeItem *parent = nullptr);
    ~StreamTreeItem();

    void appendChild(StreamTreeItem *child);
    StreamTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QString data(int column) const;
    int row() const;
    StreamTreeItem *parentItem();

private:
    QVector<StreamTreeItem*> m_childItems;
    QString m_name;
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

    void setupModelData();

private:
    StreamTreeItem *rootItem;
};

#endif // STREAMTREEMODEL_H 