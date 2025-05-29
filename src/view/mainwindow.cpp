#include "view/mainwindow.h"
#include "./ui_mainwindow.h"
#include "model/model.h"

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

    // Create and setup MVC components
    Model *model = new Model(this);
    controller = new Controller(model, this);

    setupDockAreaPriorities();
    createMenus();
    setupDockWidgets();
    setupConnections();
}

void MainWindow::setupConnections()
{
    // Connect controller signals
    connect(controller, &Controller::updateWindowTitle, this, &MainWindow::updateWindowTitle);
    connect(controller, &Controller::showError, this, &MainWindow::showError);
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

    // Create managers and add their actions to the menu
    sequenceManager = new SequenceWidgetManager(this);
    viewMenu->addAction(sequenceManager->getAction());
    connect(sequenceManager->getAction(), &QAction::triggered, this, &MainWindow::onSequence);

    streamsManager = new StreamsWidgetManager(this);
    viewMenu->addAction(streamsManager->getAction());
    connect(streamsManager->getAction(), &QAction::triggered, this, &MainWindow::onStreams);

    sliceManager = new SliceWidgetManager(this);
    viewMenu->addAction(sliceManager->getAction());
    connect(sliceManager->getAction(), &QAction::triggered, this, &MainWindow::onSlice);

    hexManager = new HexWidgetManager(this);
    viewMenu->addAction(hexManager->getAction());
    connect(hexManager->getAction(), &QAction::triggered, this, &MainWindow::onHex);

    macroblockManager = new MacroblockWidgetManager(this);
    viewMenu->addAction(macroblockManager->getAction());
    connect(macroblockManager->getAction(), &QAction::triggered, this, &MainWindow::onMacroblock);
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
    // Create dock widgets using managers
    sequenceManager->createDockWidget(tr("Sequence"), Qt::TopDockWidgetArea);
    streamsManager->createDockWidget(tr("Streams"), Qt::LeftDockWidgetArea);
    sliceManager->createDockWidget(tr("Slice"), Qt::LeftDockWidgetArea);
    hexManager->createDockWidget(tr("Hex"), Qt::BottomDockWidgetArea);
    macroblockManager->createDockWidget(tr("Macroblock"), Qt::BottomDockWidgetArea);

    // Add dock widgets to the main window
    addDockWidget(Qt::TopDockWidgetArea, sequenceManager->getDockWidget());
    addDockWidget(Qt::LeftDockWidgetArea, streamsManager->getDockWidget());
    addDockWidget(Qt::LeftDockWidgetArea, sliceManager->getDockWidget());
    addDockWidget(Qt::BottomDockWidgetArea, hexManager->getDockWidget());
    addDockWidget(Qt::BottomDockWidgetArea, macroblockManager->getDockWidget());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOpenFile()
{
    controller->openFile();
}

void MainWindow::onCloseFile()
{
    controller->closeFile();
}

void MainWindow::onAboutUs()
{
    QMessageBox::information(this, tr("About us"), tr("Legilimens"));
}

void MainWindow::onStreams()
{
    if (streamsManager && streamsManager->getDockWidget()) {
        bool willShow = !streamsManager->getDockWidget()->isVisible();
        streamsManager->getDockWidget()->setVisible(willShow);
        streamsManager->getAction()->setChecked(willShow);
    }
}

void MainWindow::onSlice()
{
    if (sliceManager && sliceManager->getDockWidget()) {
        bool willShow = !sliceManager->getDockWidget()->isVisible();
        sliceManager->getDockWidget()->setVisible(willShow);
        sliceManager->getAction()->setChecked(willShow);
    }
}

void MainWindow::onHex()
{
    if (hexManager && hexManager->getDockWidget()) {
        bool willShow = !hexManager->getDockWidget()->isVisible();
        hexManager->getDockWidget()->setVisible(willShow);
        hexManager->getAction()->setChecked(willShow);
    }
}

void MainWindow::onMacroblock()
{
    if (macroblockManager && macroblockManager->getDockWidget()) {
        bool willShow = !macroblockManager->getDockWidget()->isVisible();
        macroblockManager->getDockWidget()->setVisible(willShow);
        macroblockManager->getAction()->setChecked(willShow);
    }
}

void MainWindow::onPlay()
{
    controller->play();
}

void MainWindow::onPause()
{
    controller->pause();
}

void MainWindow::onStop()
{
    controller->stop();
}

void MainWindow::onResume()
{
    controller->resume();
}

void MainWindow::onSequence()
{
    if (sequenceManager && sequenceManager->getDockWidget()) {
        bool willShow = !sequenceManager->getDockWidget()->isVisible();
        sequenceManager->getDockWidget()->setVisible(willShow);
        sequenceManager->getAction()->setChecked(willShow);
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    // Add any custom resize handling here if needed
}

void MainWindow::updateWindowTitle(const QString &title)
{
    setWindowTitle(title);
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, tr("Error"), message);
}

