#ifndef MACROBLOCKWIDGETMANAGER_H
#define MACROBLOCKWIDGETMANAGER_H

#include "common/basewidgetmanager.h"

class MacroblockWidgetManager : public BaseWidgetManager
{
    Q_OBJECT

public:
    explicit MacroblockWidgetManager(QWidget *parent = nullptr);
    void updateContent() override;

protected:
    void setupContentWidget() override;
    void setupConnections() override;
};

#endif // MACROBLOCKWIDGETMANAGER_H 