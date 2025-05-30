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
#include <QShowEvent>
#include <QMimeData>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Legilimens");

    // Enable drag and drop
    setAcceptDrops(true);

    // Create and setup MVC components
    Model *model = new Model(this);
    controller = new Controller(model, this);

    setupDockAreaPriorities();
    createMenus();
    setupDockWidgets();
    setupConnections();

    // Show window maximized
    showMaximized();
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
    if (openFileAction) {
        connect(openFileAction, &QAction::triggered, this, &MainWindow::onOpenFile);
        fileMenu->addAction(openFileAction);
    }

    QAction *closeFileAction = new QAction(tr("&Close"), this);
    if (closeFileAction) {
        connect(closeFileAction, &QAction::triggered, this, &MainWindow::onCloseFile);
        fileMenu->addAction(closeFileAction);
    }

    QAction *exitAction = new QAction(tr("E&xit"), this);
    if (exitAction) {
        connect(exitAction, &QAction::triggered, this, &QWidget::close);
        fileMenu->addAction(exitAction);
    }
}

void MainWindow::createViewMenu()
{
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    // Create managers first
    sequenceManager = new SequenceWidgetManager(this);
    streamsManager = new StreamsWidgetManager(this);
    sliceManager = new SliceWidgetManager(this);
    hexManager = new HexWidgetManager(this);
    macroblockManager = new MacroblockWidgetManager(this);

    // Create checkable actions for each view
    QAction *sequenceAction = new QAction(tr("&Sequence"), this);
    sequenceAction->setCheckable(true);
    sequenceAction->setChecked(true);
    viewMenu->addAction(sequenceAction);
    connect(sequenceAction, &QAction::triggered, this, &MainWindow::onSequence);

    QAction *streamsAction = new QAction(tr("&Streams"), this);
    streamsAction->setCheckable(true);
    streamsAction->setChecked(true);
    viewMenu->addAction(streamsAction);
    connect(streamsAction, &QAction::triggered, this, &MainWindow::onStreams);

    QAction *sliceAction = new QAction(tr("&Slice"), this);
    sliceAction->setCheckable(true);
    sliceAction->setChecked(true);
    viewMenu->addAction(sliceAction);
    connect(sliceAction, &QAction::triggered, this, &MainWindow::onSlice);

    QAction *hexAction = new QAction(tr("&Hex"), this);
    hexAction->setCheckable(true);
    hexAction->setChecked(true);
    viewMenu->addAction(hexAction);
    connect(hexAction, &QAction::triggered, this, &MainWindow::onHex);

    QAction *macroblockAction = new QAction(tr("&Macroblock"), this);
    macroblockAction->setCheckable(true);
    macroblockAction->setChecked(true);
    viewMenu->addAction(macroblockAction);
    connect(macroblockAction, &QAction::triggered, this, &MainWindow::onMacroblock);

    // Store the actions in the managers
    if (sequenceManager) sequenceManager->setAction(sequenceAction);
    if (streamsManager) streamsManager->setAction(streamsAction);
    if (sliceManager) sliceManager->setAction(sliceAction);
    if (hexManager) hexManager->setAction(hexAction);
    if (macroblockManager) macroblockManager->setAction(macroblockAction);
}

void MainWindow::createPlaybackMenu()
{
    QMenu *playbackMenu = menuBar()->addMenu(tr("&Playback"));
    
    QAction *playbackAction = new QAction(tr("&Play"), this);
    if (playbackAction) {
        connect(playbackAction, &QAction::triggered, this, &MainWindow::onPlay);
        playbackMenu->addAction(playbackAction);
    }

    QAction *pauseAction = new QAction(tr("&Pause"), this);
    if (pauseAction) {
        connect(pauseAction, &QAction::triggered, this, &MainWindow::onPause);
        playbackMenu->addAction(pauseAction);
    }

    QAction *stopAction = new QAction(tr("&Stop"), this);
    if (stopAction) {
        connect(stopAction, &QAction::triggered, this, &MainWindow::onStop);
        playbackMenu->addAction(stopAction);
    }

    QAction *resumeAction = new QAction(tr("&Resume"), this);
    if (resumeAction) {
        connect(resumeAction, &QAction::triggered, this, &MainWindow::onResume);
        playbackMenu->addAction(resumeAction);
    }
}

void MainWindow::createHelpMenu()
{
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutUsAction = new QAction(tr("&About us"), this);
    if (aboutUsAction) {
        connect(aboutUsAction, &QAction::triggered, this, &MainWindow::onAboutUs);
        helpMenu->addAction(aboutUsAction);
    }
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

    // Set initial sizes
    int windowWidth = width();
    int windowHeight = height();

    // Set Stream and Slice views to 25% of window width
    int sideWidth = static_cast<int>(windowWidth * 0.25);
    if (streamsManager && streamsManager->getDockWidget()) {
        streamsManager->getDockWidget()->setMinimumWidth(sideWidth);
        streamsManager->getDockWidget()->resize(sideWidth, streamsManager->getDockWidget()->height());
    }
    if (sliceManager && sliceManager->getDockWidget()) {
        sliceManager->getDockWidget()->setMinimumWidth(sideWidth);
        sliceManager->getDockWidget()->resize(sideWidth, sliceManager->getDockWidget()->height());
    }

    // Set Sequence view to 20% of window height
    int sequenceHeight = static_cast<int>(windowHeight * 0.2);
    if (sequenceManager && sequenceManager->getDockWidget()) {
        sequenceManager->getDockWidget()->setMinimumHeight(sequenceHeight);
        sequenceManager->getDockWidget()->resize(sequenceManager->getDockWidget()->width(), sequenceHeight);
    }

    // Set Hex and Macroblock views to 50% of window height
    int bottomHeight = static_cast<int>(windowHeight * 0.5);
    if (hexManager && hexManager->getDockWidget()) {
        hexManager->getDockWidget()->setMinimumHeight(bottomHeight);
        hexManager->getDockWidget()->resize(hexManager->getDockWidget()->width(), bottomHeight);
    }
    if (macroblockManager && macroblockManager->getDockWidget()) {
        macroblockManager->getDockWidget()->setMinimumHeight(bottomHeight);
        macroblockManager->getDockWidget()->resize(macroblockManager->getDockWidget()->width(), bottomHeight);
    }
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
        if (streamsManager->getAction()) {
            streamsManager->getAction()->setChecked(willShow);
        }
    }
}

void MainWindow::onSlice()
{
    if (sliceManager && sliceManager->getDockWidget()) {
        bool willShow = !sliceManager->getDockWidget()->isVisible();
        sliceManager->getDockWidget()->setVisible(willShow);
        if (sliceManager->getAction()) {
            sliceManager->getAction()->setChecked(willShow);
        }
    }
}

void MainWindow::onHex()
{
    if (hexManager && hexManager->getDockWidget()) {
        bool willShow = !hexManager->getDockWidget()->isVisible();
        hexManager->getDockWidget()->setVisible(willShow);
        if (hexManager->getAction()) {
            hexManager->getAction()->setChecked(willShow);
        }
    }
}

void MainWindow::onMacroblock()
{
    if (macroblockManager && macroblockManager->getDockWidget()) {
        bool willShow = !macroblockManager->getDockWidget()->isVisible();
        macroblockManager->getDockWidget()->setVisible(willShow);
        if (macroblockManager->getAction()) {
            macroblockManager->getAction()->setChecked(willShow);
        }
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
        if (sequenceManager->getAction()) {
            sequenceManager->getAction()->setChecked(willShow);
        }
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

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    setupInitialWidgetSizes();
}

void MainWindow::setupInitialWidgetSizes()
{
    // Get the main window dimensions
    int windowWidth = width();
    int windowHeight = height();

    // Set Stream and Slice views to 25% of window width
    int sideWidth = static_cast<int>(windowWidth * 0.25);
    if (streamsManager && streamsManager->getDockWidget()) {
        streamsManager->getDockWidget()->setMinimumWidth(sideWidth);
        streamsManager->getDockWidget()->resize(sideWidth, streamsManager->getDockWidget()->height());
    }
    if (sliceManager && sliceManager->getDockWidget()) {
        sliceManager->getDockWidget()->setMinimumWidth(sideWidth);
        sliceManager->getDockWidget()->resize(sideWidth, sliceManager->getDockWidget()->height());
    }

    // Set Sequence view to 20% of window height
    int sequenceHeight = static_cast<int>(windowHeight * 0.2);
    if (sequenceManager && sequenceManager->getDockWidget()) {
        sequenceManager->getDockWidget()->setMinimumHeight(sequenceHeight);
        sequenceManager->getDockWidget()->resize(sequenceManager->getDockWidget()->width(), sequenceHeight);
    }

    // Set Hex and Macroblock views to 50% of window height
    int bottomHeight = static_cast<int>(windowHeight * 0.5);
    if (hexManager && hexManager->getDockWidget()) {
        hexManager->getDockWidget()->setMinimumHeight(bottomHeight);
        hexManager->getDockWidget()->resize(hexManager->getDockWidget()->width(), bottomHeight);
    }
    if (macroblockManager && macroblockManager->getDockWidget()) {
        macroblockManager->getDockWidget()->setMinimumHeight(bottomHeight);
        macroblockManager->getDockWidget()->resize(macroblockManager->getDockWidget()->width(), bottomHeight);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    // Accept the event if it contains URLs (files)
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    qDebug() << "Drop event received";
    // Check if the dropped data contains URLs
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        qDebug() << "URL list: " << urlList;
        // Get the first file URL
        if (!urlList.isEmpty()) {
            QString filePath = urlList.first().toLocalFile();
            if (!filePath.isEmpty()) {
                // Use the controller to open the file
                if (!controller->openFile(filePath)) {
                    showError("Failed to open file");
                }
            }
        }
    }
}

