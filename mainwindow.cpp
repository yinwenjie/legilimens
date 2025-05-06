#include "mainwindow.h"
#include <QFileDialog>
#include "./ui_mainwindow.h"

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

    // Create an Exit action
    QAction *exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // Add the Exit action to the File menu
    fileMenu->addAction(exitAction);
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
