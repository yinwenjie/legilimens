#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDockWidget>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Legilimens");

    setupDockAreaPriorities();
    createMenus();
    setupDockWidgets();
}

void MainWindow::setupDockAreaPriorities()
{
    // Set dock area priorities
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);    // Top area takes precedence over left
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);  // Top area takes precedence over right
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea); // Left area takes precedence over bottom
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea); // Right area takes precedence over bottom
}

void MainWindow::createMenus()
{
    createFileMenu();
    createViewMenu();
    createPlaybackMenu();
    createHelpMenu();
}

void MainWindow::createFileMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openFileAction = new QAction(tr("&Open File"), this);
    connect(openFileAction, &QAction::triggered, this, &MainWindow::onOpenFile);
    fileMenu->addAction(openFileAction);

    QAction *closeFileAction = new QAction(tr("&Close"), this);
    connect(closeFileAction, &QAction::triggered, this, &MainWindow::onCloseFile);
    fileMenu->addAction(closeFileAction);

    QAction *exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
}

void MainWindow::createViewMenu()
{
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    sequenceAction = new QAction(tr("&Sequence"), this);
    sequenceAction->setCheckable(true);
    connect(sequenceAction, &QAction::triggered, this, &MainWindow::onSequence);
    viewMenu->addAction(sequenceAction);

    streamsAction = new QAction(tr("&Streams"), this);
    streamsAction->setCheckable(true);
    connect(streamsAction, &QAction::triggered, this, &MainWindow::onStreams);
    viewMenu->addAction(streamsAction);

    sliceAction = new QAction(tr("&Slice"), this);
    sliceAction->setCheckable(true);
    connect(sliceAction, &QAction::triggered, this, &MainWindow::onSlice);
    viewMenu->addAction(sliceAction);

    hexAction = new QAction(tr("&Hex"), this);
    hexAction->setCheckable(true);
    connect(hexAction, &QAction::triggered, this, &MainWindow::onHex);
    viewMenu->addAction(hexAction);

    macroblockAction = new QAction(tr("&Macroblock"), this);
    macroblockAction->setCheckable(true);
    connect(macroblockAction, &QAction::triggered, this, &MainWindow::onMacroblock);
    viewMenu->addAction(macroblockAction);
}

void MainWindow::createPlaybackMenu()
{
    QMenu *playbackMenu = menuBar()->addMenu(tr("&Playback"));
    
    QAction *playbackAction = new QAction(tr("&Play"), this);
    connect(playbackAction, &QAction::triggered, this, &MainWindow::onPlay);
    playbackMenu->addAction(playbackAction);

    QAction *pauseAction = new QAction(tr("&Pause"), this);
    connect(pauseAction, &QAction::triggered, this, &MainWindow::onPause);
    playbackMenu->addAction(pauseAction);

    QAction *stopAction = new QAction(tr("&Stop"), this);
    connect(stopAction, &QAction::triggered, this, &MainWindow::onStop);
    playbackMenu->addAction(stopAction);

    QAction *resumeAction = new QAction(tr("&Resume"), this);
    connect(resumeAction, &QAction::triggered, this, &MainWindow::onResume);
    playbackMenu->addAction(resumeAction);
}

void MainWindow::createHelpMenu()
{
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutUsAction = new QAction(tr("&About us"), this);
    connect(aboutUsAction, &QAction::triggered, this, &MainWindow::onAboutUs);
    helpMenu->addAction(aboutUsAction);
}

void MainWindow::setupDockWidgets()
{
    sequenceDock = createDockWidget(tr("Sequence"), sequenceAction, Qt::TopDockWidgetArea);
    streamsDock = createDockWidget(tr("Streams"), streamsAction, Qt::LeftDockWidgetArea);
    sliceDock = createDockWidget(tr("Slice"), sliceAction, Qt::LeftDockWidgetArea);
    hexDock = createDockWidget(tr("Hex"), hexAction, Qt::BottomDockWidgetArea);
    macroblockDock = createDockWidget(tr("Macroblock"), macroblockAction, Qt::BottomDockWidgetArea);
}

QDockWidget* MainWindow::createDockWidget(const QString& title, QAction* action, Qt::DockWidgetArea area)
{
    QDockWidget* dock = new QDockWidget(title, this);
    QFrame* frame = new QFrame(dock);
    frame->setFrameShape(QFrame::StyledPanel);
    QLabel* label = new QLabel(tr("%1 content goes here").arg(title), frame);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    layout->addWidget(label);
    frame->setLayout(layout);
    dock->setWidget(frame);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(area, dock);
    dock->setVisible(true);
    action->setChecked(true);

    // Store the action in the map
    dockActionMap[dock] = action;

    // Connect visibility changed signal
    connect(dock, &QDockWidget::visibilityChanged, this, [this, dock](bool visible) {
        if (dockActionMap.contains(dock)) {
            dockActionMap[dock]->setChecked(visible);
        }
    });

    return dock;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*)"));
    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);
        qint64 size = fileInfo.size();
        qDebug() << "Selected file:" << fileName << ", Size:" << size << "bytes";

        // Update the window title to include the file path
        setWindowTitle(QString("Legilimens - %1").arg(fileName));
    }
}

void MainWindow::onCloseFile()
{
    setWindowTitle("Legilimens"); // Reset to default title
}

void MainWindow::onAboutUs()
{
    QMessageBox::information(this, tr("About us"), tr("Legilimens"));
}

void MainWindow::onStreams()
{
    if (!streamsDock) {
        streamsDock = createDockWidget(tr("Streams"), streamsAction, Qt::LeftDockWidgetArea);
    }
    bool willShow = !streamsDock->isVisible();
    streamsDock->setVisible(willShow);
    streamsAction->setChecked(willShow);
}

void MainWindow::onSlice()
{
    if (!sliceDock) {
        sliceDock = createDockWidget(tr("Slice"), sliceAction, Qt::RightDockWidgetArea);
    }
    bool willShow = !sliceDock->isVisible();
    sliceDock->setVisible(willShow);
    sliceAction->setChecked(willShow);
}

void MainWindow::onHex()
{
    if (!hexDock) {
        hexDock = createDockWidget(tr("Hex"), hexAction, Qt::RightDockWidgetArea);
    }
    bool willShow = !hexDock->isVisible();
    hexDock->setVisible(willShow);
    hexAction->setChecked(willShow);
}

void MainWindow::onMacroblock()
{
    if (!macroblockDock) {
        macroblockDock = createDockWidget(tr("Macroblock"), macroblockAction, Qt::RightDockWidgetArea);
    }
    bool willShow = !macroblockDock->isVisible();
    macroblockDock->setVisible(willShow);
    macroblockAction->setChecked(willShow);
}

void MainWindow::onPlay()
{

}

void MainWindow::onPause()
{

}

void MainWindow::onStop()
{

}

void MainWindow::onResume()
{

}

void MainWindow::onSequence()
{
    if (!sequenceDock) {
        sequenceDock = createDockWidget(tr("Sequence"), sequenceAction, Qt::TopDockWidgetArea);
    }
    bool willShow = !sequenceDock->isVisible();
    sequenceDock->setVisible(willShow);
    sequenceAction->setChecked(willShow);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    // Add any custom resize handling here if needed
}

