#ifndef SEQUENCEWIDGETMANAGER_H
#define SEQUENCEWIDGETMANAGER_H

#include "common/basewidgetmanager.h"

class SequenceWidgetManager : public BaseWidgetManager
{
    Q_OBJECT

public:
    explicit SequenceWidgetManager(QWidget *parent = nullptr);
    void updateContent() override;
    void clearContent() override;

protected:
    void setupContentWidget() override;
    void setupConnections() override;
};

#endif // SEQUENCEWIDGETMANAGER_H 