#ifndef DOCKWIDGETMANAGER_H
#define DOCKWIDGETMANAGER_H

#include <QObject>
#include <QDockWidget>
#include <QAction>
#include <QString>

class DockWidgetManager : public QObject
{
    Q_OBJECT

public:
    explicit DockWidgetManager(QWidget* parent = nullptr);
    ~DockWidgetManager();

    // Create and setup a new dock widget with its action
    QDockWidget* createDockWidget(const QString& title, Qt::DockWidgetArea area);
    
    // Get the action associated with a dock widget
    QAction* getAction() const { return action; }
    
    // Get the dock widget
    QDockWidget* getDockWidget() const { return dockWidget; }

private:
    QDockWidget* dockWidget;
    QAction* action;

    void setupDockWidget(const QString& title, Qt::DockWidgetArea area);
    void setupAction(const QString& title);
};

#endif // DOCKWIDGETMANAGER_H 