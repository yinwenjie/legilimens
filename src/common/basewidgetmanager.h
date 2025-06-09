#ifndef BASEWIDGETMANAGER_H
#define BASEWIDGETMANAGER_H

#include <QObject>
#include <QAction>
#include <QDockWidget>
#include <QWidget>

class BaseWidgetManager : public QObject
{
    Q_OBJECT

public:
    explicit BaseWidgetManager(QWidget *parent = nullptr);
    virtual ~BaseWidgetManager();

    // Common interface
    QAction* getAction() const { return action; }
    QDockWidget* getDockWidget() const { return dockWidget; }
    void setAction(QAction* newAction) { action = newAction; }
    void createDockWidget(const QString &title, Qt::DockWidgetArea area);
    virtual void updateContent() = 0;  // Pure virtual function for content updates
    virtual void clearContent() = 0;   // Pure virtual function for clearing content

protected:
    QAction *action;
    QDockWidget *dockWidget;
    QWidget *contentWidget;

    // Helper methods
    virtual void setupContentWidget() = 0;  // Pure virtual function for widget setup
    virtual void setupConnections() = 0;    // Pure virtual function for signal connections
};

#endif // BASEWIDGETMANAGER_H 