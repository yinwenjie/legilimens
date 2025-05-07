#include "mainwindow.h"
#include <QFileDialog>
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDockWidget>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Legilimens");

    // Create a File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    // Create an Open File action
    QAction *openFileAction = new QAction(tr("&Open File"), this);
    connect(openFileAction, &QAction::triggered, this, &MainWindow::onOpenFile);
    fileMenu->addAction(openFileAction);

    // Create a Close action
    QAction *closeFileAction = new QAction(tr("&Close"), this);
    connect(closeFileAction, &QAction::triggered, this, &MainWindow::onCloseFile);
    fileMenu->addAction(closeFileAction);

    // Create an Exit action
    QAction *exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // Add the Exit action to the File menu
    fileMenu->addAction(exitAction);

    // Create a View menu
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    
    // Streams view
    streamsAction = new QAction(tr("&Streams"), this);
    streamsAction->setCheckable(true);
    connect(streamsAction, &QAction::triggered, this, &MainWindow::onStreams);
    viewMenu->addAction(streamsAction);

    // Slice view
    sliceAction = new QAction(tr("&Slice"), this);
    sliceAction->setCheckable(true);
    connect(sliceAction, &QAction::triggered, this, &MainWindow::onSlice);
    viewMenu->addAction(sliceAction);

    // Hex view
    hexAction = new QAction(tr("&Hex"), this);
    hexAction->setCheckable(true);
    connect(hexAction, &QAction::triggered, this, &MainWindow::onHex);
    viewMenu->addAction(hexAction);

    // Macroblock view
    macroblockAction = new QAction(tr("&Macroblock"), this);
    macroblockAction->setCheckable(true);
    connect(macroblockAction, &QAction::triggered, this, &MainWindow::onMacroblock);
    viewMenu->addAction(macroblockAction);

    // Create and show the streams dock widget by default
    streamsDock = new QDockWidget(tr("Streams"), this);
    QFrame* frame = new QFrame(streamsDock);
    frame->setFrameShape(QFrame::StyledPanel);
    QLabel* label = new QLabel(tr("Streams content goes here"), frame);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    layout->addWidget(label);
    frame->setLayout(layout);
    streamsDock->setWidget(frame);
    streamsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    streamsDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::LeftDockWidgetArea, streamsDock);
    streamsDock->setVisible(true);
    streamsAction->setChecked(true);

    // Create and show the slice dock widget by default
    sliceDock = new QDockWidget(tr("Slice"), this);
    QFrame* sliceFrame = new QFrame(sliceDock);
    sliceFrame->setFrameShape(QFrame::StyledPanel);
    QLabel* sliceLabel = new QLabel(tr("Slice content goes here"), sliceFrame);
    QVBoxLayout* sliceLayout = new QVBoxLayout(sliceFrame);
    sliceLayout->addWidget(sliceLabel);
    sliceFrame->setLayout(sliceLayout);
    sliceDock->setWidget(sliceFrame);
    sliceDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    sliceDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, sliceDock);
    sliceDock->setVisible(true);
    sliceAction->setChecked(true);

    // Create and show the hex dock widget by default
    hexDock = new QDockWidget(tr("Hex"), this);
    QFrame* hexFrame = new QFrame(hexDock);
    hexFrame->setFrameShape(QFrame::StyledPanel);
    QLabel* hexLabel = new QLabel(tr("Hex content goes here"), hexFrame);
    QVBoxLayout* hexLayout = new QVBoxLayout(hexFrame);
    hexLayout->addWidget(hexLabel);
    hexFrame->setLayout(hexLayout);
    hexDock->setWidget(hexFrame);
    hexDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    hexDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, hexDock);
    hexDock->setVisible(true);
    hexAction->setChecked(true);

    // Create and show the macroblock dock widget by default
    macroblockDock = new QDockWidget(tr("Macroblock"), this);
    QFrame* macroblockFrame = new QFrame(macroblockDock);
    macroblockFrame->setFrameShape(QFrame::StyledPanel);
    QLabel* macroblockLabel = new QLabel(tr("Macroblock content goes here"), macroblockFrame);
    QVBoxLayout* macroblockLayout = new QVBoxLayout(macroblockFrame);
    macroblockLayout->addWidget(macroblockLabel);
    macroblockFrame->setLayout(macroblockLayout);
    macroblockDock->setWidget(macroblockFrame);
    macroblockDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    macroblockDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, macroblockDock);
    macroblockDock->setVisible(true);
    macroblockAction->setChecked(true);

    connect(streamsDock, &QDockWidget::visibilityChanged, this, [this](bool visible){
        streamsAction->setChecked(visible);
    });

    connect(sliceDock, &QDockWidget::visibilityChanged, this, [this](bool visible){
        sliceAction->setChecked(visible);
    });

    connect(hexDock, &QDockWidget::visibilityChanged, this, [this](bool visible){
        hexAction->setChecked(visible);
    });

    connect(macroblockDock, &QDockWidget::visibilityChanged, this, [this](bool visible){
        macroblockAction->setChecked(visible);
    });

    // Create a Playback menu
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

    // Create a Help menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutUsAction = new QAction(tr("&About us"), this);
    connect(aboutUsAction, &QAction::triggered, this, &MainWindow::onAboutUs);
    helpMenu->addAction(aboutUsAction);
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
    // Create a dock widget if it doesn't exist
    if (!streamsDock) {
        streamsDock = new QDockWidget(tr("Streams"), this);
        QFrame* frame = new QFrame(streamsDock);
        frame->setFrameShape(QFrame::StyledPanel);
        QLabel* label = new QLabel(tr("Streams content goes here"), frame);
        QVBoxLayout* layout = new QVBoxLayout(frame);
        layout->addWidget(label);
        frame->setLayout(layout);
        streamsDock->setWidget(frame);
        streamsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        streamsDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        addDockWidget(Qt::LeftDockWidgetArea, streamsDock);

        connect(streamsDock, &QDockWidget::visibilityChanged, this, [this](bool visible){
            streamsAction->setChecked(visible);
        });
    }
    bool willShow = !streamsDock->isVisible();
    streamsDock->setVisible(willShow);
    streamsAction->setChecked(willShow);
}

void MainWindow::onSlice()
{
    if (!sliceDock) {
        sliceDock = new QDockWidget(tr("Slice"), this);
        QFrame* frame = new QFrame(sliceDock);
        frame->setFrameShape(QFrame::StyledPanel);
        QLabel* label = new QLabel(tr("Slice content goes here"), frame);
        QVBoxLayout* layout = new QVBoxLayout(frame);
        layout->addWidget(label);
        frame->setLayout(layout);
        sliceDock->setWidget(frame);
        sliceDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        sliceDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        addDockWidget(Qt::RightDockWidgetArea, sliceDock);

        connect(sliceDock, &QDockWidget::visibilityChanged, this, [this](bool visible){
            sliceAction->setChecked(visible);
        });
    }
    bool willShow = !sliceDock->isVisible();
    sliceDock->setVisible(willShow);
    sliceAction->setChecked(willShow);
}

void MainWindow::onHex()
{
    if (!hexDock) {
        hexDock = new QDockWidget(tr("Hex"), this);
        QFrame* frame = new QFrame(hexDock);
        frame->setFrameShape(QFrame::StyledPanel);
        QLabel* label = new QLabel(tr("Hex content goes here"), frame);
        QVBoxLayout* layout = new QVBoxLayout(frame);
        layout->addWidget(label);
        frame->setLayout(layout);
        hexDock->setWidget(frame);
        hexDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        hexDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        addDockWidget(Qt::RightDockWidgetArea, hexDock);

        connect(hexDock, &QDockWidget::visibilityChanged, this, [this](bool visible){
            hexAction->setChecked(visible);
        });
    }
    bool willShow = !hexDock->isVisible();
    hexDock->setVisible(willShow);
    hexAction->setChecked(willShow);
}

void MainWindow::onMacroblock()
{
    if (!macroblockDock) {
        macroblockDock = new QDockWidget(tr("Macroblock"), this);
        QFrame* frame = new QFrame(macroblockDock);
        frame->setFrameShape(QFrame::StyledPanel);
        QLabel* label = new QLabel(tr("Macroblock content goes here"), frame);
        QVBoxLayout* layout = new QVBoxLayout(frame);
        layout->addWidget(label);
        frame->setLayout(layout);
        macroblockDock->setWidget(frame);
        macroblockDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        macroblockDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        addDockWidget(Qt::RightDockWidgetArea, macroblockDock);

        connect(macroblockDock, &QDockWidget::visibilityChanged, this, [this](bool visible){
            macroblockAction->setChecked(visible);
        });
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


