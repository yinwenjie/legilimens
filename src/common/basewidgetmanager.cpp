#include "basewidgetmanager.h"
#include <QVBoxLayout>
#include <QFrame>

BaseWidgetManager::BaseWidgetManager(QWidget *parent)
    : QObject(parent)
    , action(nullptr)
    , dockWidget(nullptr)
    , contentWidget(nullptr)
{
}

BaseWidgetManager::~BaseWidgetManager()
{
    delete dockWidget;
    delete action;
}

void BaseWidgetManager::createDockWidget(const QString &title, Qt::DockWidgetArea area)
{
    // Create the dock widget
    dockWidget = new QDockWidget(title, qobject_cast<QWidget*>(parent()));
    dockWidget->setAllowedAreas(area);

    // Create a frame to hold the content widget and provide a border
    QFrame *frame = new QFrame(dockWidget);
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    frame->setStyleSheet("QFrame { background-color: #f0f0f0; border: 1px solid #a0a0a0; }");

    // Create the content widget
    contentWidget = new QWidget(frame);
    QVBoxLayout *frameLayout = new QVBoxLayout(frame);
    frameLayout->setContentsMargins(2, 2, 2, 2);  // Add small margin inside the frame
    frameLayout->addWidget(contentWidget);
    
    dockWidget->setWidget(frame);

    // Setup the content widget
    setupContentWidget();
    setupConnections();

    // Create the action
    action = new QAction(title, this);
    action->setCheckable(true);
    action->setChecked(dockWidget->isVisible());

    // Connect the action to the dock widget visibility
    connect(action, &QAction::triggered, dockWidget, &QDockWidget::setVisible);
    connect(dockWidget, &QDockWidget::visibilityChanged, action, &QAction::setChecked);
} 