#ifndef SLICEWIDGETMANAGER_H
#define SLICEWIDGETMANAGER_H

#include "common/basewidgetmanager.h"

class SliceWidgetManager : public BaseWidgetManager
{
    Q_OBJECT

public:
    explicit SliceWidgetManager(QWidget *parent = nullptr);
    void updateContent() override;
    void clearContent() override;

protected:
    void setupContentWidget() override;
    void setupConnections() override;
};

#endif // SLICEWIDGETMANAGER_H 