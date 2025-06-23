#ifndef SLICETREEMODEL_H
#define SLICETREEMODEL_H

#include <QAbstractItemModel>
#include <QString>
#include <QVector>
#include <QVariant>
#include <QMap>
#include <QList>

// Forward declarations
struct SliceInfo;

/**
 * @brief The SliceTreeItem class represents a node in the slice tree view
 * 
 * This class is used to build a hierarchical tree structure for displaying
 * slice information in a QTreeView. Each item can have a parent and multiple
 * children, forming a tree.
 */
class SliceTreeItem
{
public:
    /**
     * @brief Construct a new Slice Tree Item
     * 
     * @param name The name/label for this item
     * @param value The value to display (optional)
     * @param parent The parent item (nullptr for root)
     */
    explicit SliceTreeItem(const QString &name, const QString &value = QString(), SliceTreeItem *parent = nullptr);
    ~SliceTreeItem();

    /**
     * @brief Add a child item to this item
     * @param child The child item to add
     */
    void appendChild(SliceTreeItem *child);
    
    /**
     * @brief Get a child at the specified row
     * @param row The row index
     * @return SliceTreeItem* The child item or nullptr if invalid
     */
    SliceTreeItem *child(int row);
    
    /**
     * @brief Get the number of children
     * @return int Child count
     */
    int childCount() const;
    
    /**
     * @brief Get the number of columns
     * @return int Column count
     */
    int columnCount() const;
    
    /**
     * @brief Get the data for a specific column
     * @param column The column index
     * @return QVariant The data to display
     */
    QVariant data(int column) const;
    
    /**
     * @brief Get the row index of this item in its parent
     * @return int Row index
     */
    int row() const;
    
    /**
     * @brief Get the parent item
     * @return SliceTreeItem* The parent item
     */
    SliceTreeItem *parentItem();
    
    /**
     * @brief Set the name and value data
     * @param name The name to set
     * @param value The value to set
     */
    void setData(const QString &name, const QVariant &value);

private:
    QVector<SliceTreeItem*> m_childItems;  ///< Child items
    QString m_name;                        ///< Item name/label
    QVariant m_value;                      ///< Item value
    SliceTreeItem *m_parentItem;           ///< Parent item
};

/**
 * @brief The SliceTreeModel class provides a model for displaying slice data in a tree view
 * 
 * This model organizes slice information in a hierarchical structure for display in a QTreeView.
 * It implements the Model component of the Model-View-Controller pattern for slice data.
 */
class SliceTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Slice Tree Model
     * @param parent The parent QObject
     */
    explicit SliceTreeModel(QObject *parent = nullptr);
    ~SliceTreeModel();

    // QAbstractItemModel implementation
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * @brief Update the model with new slice data (replaces existing data)
     * @param slices The list of slice information to display
     */
    void updateSliceData(const QList<SliceInfo> &slices);
    
    /**
     * @brief Append new slices to the existing data
     * @param slices The list of new slice information to append
     */
    void appendSliceData(const QList<SliceInfo> &slices);
    
    /**
     * @brief Clear all slice data from the model
     */
    void clearSliceData();
    
    /**
     * @brief Get the total number of slices in the model
     * @return int The total slice count
     */
    int getSliceCount() const;

private:
    SliceTreeItem *rootItem;  ///< Root item of the tree
    
    // Stream type counters for organizing slices
    int videoSliceCount;
    int audioSliceCount;
    int otherSliceCount;
    
    // Maps to organize slices by stream
    QMap<int, SliceTreeItem*> streamItems;
    
    // Storage for accumulated slices
    QList<SliceInfo> accumulatedSlices;
    
    // Helper methods
    /**
     * @brief Set up initial model data
     */
    void setupModelData();
    
    /**
     * @brief Rebuild the tree model from accumulated slices
     */
    void rebuildTreeFromSlices();
    
    /**
     * @brief Create a slice item with all properties
     * @param sliceInfo The slice information
     * @param parent The parent item
     * @return SliceTreeItem* The created slice item
     */
    SliceTreeItem* createSliceItem(const SliceInfo &sliceInfo, SliceTreeItem *parent);
    
    /**
     * @brief Add a property item to a parent item
     * @param parent The parent item
     * @param name The property name
     * @param value The property value
     */
    void addPropertyItem(SliceTreeItem *parent, const QString &name, const QVariant &value);
    
    /**
     * @brief Get a formatted timestamp string
     * @param timestamp The timestamp value
     * @return QString The formatted timestamp
     */
    QString formatTimestamp(int64_t timestamp) const;
};

#endif // SLICETREEMODEL_H 