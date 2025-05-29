#ifndef HEXWIDGETMANAGER_H
#define HEXWIDGETMANAGER_H

#include "common/basewidgetmanager.h"

class HexWidgetManager : public BaseWidgetManager
{
    Q_OBJECT

public:
    explicit HexWidgetManager(QWidget *parent = nullptr);
    void updateContent() override;

protected:
    void setupContentWidget() override;
    void setupConnections() override;
};

#endif // HEXWIDGETMANAGER_H 