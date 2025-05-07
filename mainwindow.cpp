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
    streamsAction = new QAction(tr("&Streams"), this);
    streamsAction->setCheckable(true);
    connect(streamsAction, &QAction::triggered, this, &MainWindow::onStreams);
    viewMenu->addAction(streamsAction);

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

    connect(streamsDock, &QDockWidget::visibilityChanged, this, [this](bool visible){
        streamsAction->setChecked(visible);
    });

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