#include "dockwidgetmanager.h"
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

DockWidgetManager::DockWidgetManager(QWidget* parent)
    : QObject(parent)
    , dockWidget(nullptr)
    , action(nullptr)
{
}

DockWidgetManager::~DockWidgetManager()
{
    // Qt's parent-child relationship will handle cleanup
}

QDockWidget* DockWidgetManager::createDockWidget(const QString& title, Qt::DockWidgetArea area)
{
    setupDockWidget(title, area);
    setupAction(title);
    
    // Connect visibility changed signal
    connect(dockWidget, &QDockWidget::visibilityChanged, this, [this](bool visible) {
        if (action) {
            action->setChecked(visible);
        }
    });

    return dockWidget;
}

void DockWidgetManager::setupDockWidget(const QString& title, Qt::DockWidgetArea area)
{
    dockWidget = new QDockWidget(title, qobject_cast<QWidget*>(parent()));
    QFrame* frame = new QFrame(dockWidget);
    frame->setFrameShape(QFrame::StyledPanel);
    
    QLabel* label = new QLabel(tr("%1 content goes here").arg(title), frame);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    layout->addWidget(label);
    frame->setLayout(layout);
    
    dockWidget->setWidget(frame);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dockWidget->setVisible(true);
}

void DockWidgetManager::setupAction(const QString& title)
{
    action = new QAction(title, this);
    action->setCheckable(true);
    action->setChecked(true);
} 