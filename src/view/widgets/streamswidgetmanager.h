#ifndef STREAMSWIDGETMANAGER_H
#define STREAMSWIDGETMANAGER_H

#include "common/basewidgetmanager.h"
#include <QTreeView>

class StreamTreeModel;
class Controller;
struct VideoStreamInfo;
struct AudioStreamInfo;

class StreamsWidgetManager : public BaseWidgetManager
{
    Q_OBJECT

public:
    explicit StreamsWidgetManager(QWidget *parent = nullptr);
    ~StreamsWidgetManager();

    void setupContentWidget() override;
    void setupConnections() override;
    void updateContent() override;

    // Connect to controller for stream updates
    void connectToController(Controller *controller);

public slots:
    void onStreamInfoUpdated(const QList<VideoStreamInfo> &videoStreams, const QList<AudioStreamInfo> &audioStreams);

private:
    QTreeView *treeView;
    StreamTreeModel *streamModel;
    Controller *connectedController;
};

#endif // STREAMSWIDGETMANAGER_H 