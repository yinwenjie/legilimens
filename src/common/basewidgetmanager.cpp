#include "common/basewidgetmanager.h"
#include <QVBoxLayout>
#include <QFrame>

BaseWidgetManager::BaseWidgetManager(QWidget *parent)
    : QObject(parent)
    , dockWidget(nullptr)
    , action(nullptr)
    , contentWidget(nullptr)
{
}

BaseWidgetManager::~BaseWidgetManager()
{
    if (dockWidget) {
        delete dockWidget;
    }
    if (action) {
        delete action;
    }
}

void BaseWidgetManager::createDockWidget(const QString& title, Qt::DockWidgetArea area)
{
    // Create the dock widget
    dockWidget = new QDockWidget(title, qobject_cast<QWidget*>(parent()));
    dockWidget->setAllowedAreas(area);
    dockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    // Create a frame to hold the content and provide a border
    QFrame* frame = new QFrame(dockWidget);
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    frame->setStyleSheet("QFrame { background-color: #f0f0f0; border: 1px solid #a0a0a0; }");
    
    // Create a layout for the frame
    QVBoxLayout* frameLayout = new QVBoxLayout(frame);
    frameLayout->setContentsMargins(4, 4, 4, 4);  // Add small margins inside the frame
    
    // Create the content widget
    contentWidget = new QWidget(frame);
    frameLayout->addWidget(contentWidget);
    
    // Set up the content widget's layout and connections
    setupContentWidget();
    setupConnections();
    
    // Set the frame as the dock widget's widget
    dockWidget->setWidget(frame);

    // Create the action
    action = new QAction(title, this);
    action->setCheckable(true);
    action->setChecked(dockWidget->isVisible());

    // Connect the action to the dock widget visibility
    connect(action, &QAction::triggered, dockWidget, &QDockWidget::setVisible);
    connect(dockWidget, &QDockWidget::visibilityChanged, action, &QAction::setChecked);
} 