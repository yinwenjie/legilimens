#ifndef STREAMSWIDGETMANAGER_H
#define STREAMSWIDGETMANAGER_H

#include "common/basewidgetmanager.h"

class StreamsWidgetManager : public BaseWidgetManager
{
    Q_OBJECT

public:
    explicit StreamsWidgetManager(QWidget *parent = nullptr);
    void updateContent() override;

protected:
    void setupContentWidget() override;
    void setupConnections() override;
};

#endif // STREAMSWIDGETMANAGER_H 