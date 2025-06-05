#ifndef STREAMSWIDGETMANAGER_H
#define STREAMSWIDGETMANAGER_H

#include "common/basewidgetmanager.h"
#include <QTreeView>

class StreamTreeModel;

class StreamsWidgetManager : public BaseWidgetManager
{
    Q_OBJECT

public:
    explicit StreamsWidgetManager(QWidget *parent = nullptr);
    void updateContent() override;

protected:
    void setupContentWidget() override;
    void setupConnections() override;

private:
    QTreeView *treeView;
    StreamTreeModel *streamModel;
};

#endif // STREAMSWIDGETMANAGER_H 