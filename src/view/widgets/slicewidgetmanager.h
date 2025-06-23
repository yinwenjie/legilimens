#ifndef SLICEWIDGETMANAGER_H
#define SLICEWIDGETMANAGER_H

#include "common/basewidgetmanager.h"
#include <QTreeView>

class SliceTreeModel;
class Controller;
struct SliceInfo;

/**
 * @brief The SliceWidgetManager class manages the slice view widget
 * 
 * This class represents the View component of the Model-View-Controller pattern
 * for slice data. It displays slice information in a tree view.
 */
class SliceWidgetManager : public BaseWidgetManager
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Slice Widget Manager
     * @param parent The parent widget
     */
    explicit SliceWidgetManager(QWidget *parent = nullptr);
    
    /**
     * @brief Update the widget content
     */
    void updateContent() override;
    
    /**
     * @brief Clear the widget content
     */
    void clearContent() override;
    
    /**
     * @brief Connect to the controller for slice updates
     * @param controller The controller to connect to
     */
    void connectToController(Controller *controller);

public slots:
    /**
     * @brief Handle slice data updates
     * @param slices The list of slices to display
     */
    void onSlicesParsed(const QList<SliceInfo> &slices);

protected:
    /**
     * @brief Set up the content widget
     */
    void setupContentWidget() override;
    
    /**
     * @brief Set up signal connections
     */
    void setupConnections() override;

private:
    QTreeView *treeView;              ///< Tree view for displaying slices
    SliceTreeModel *sliceModel;       ///< Model for slice data
    Controller *connectedController;  ///< Connected controller
};

#endif // SLICEWIDGETMANAGER_H 