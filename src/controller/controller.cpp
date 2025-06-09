#include "controller.h"
#include <QFileDialog>
#include <QMessageBox>

Controller::Controller(MediaFileManager *mediaFileManager, QObject *parent)
    : QObject(parent)
    , mediaFileManager(mediaFileManager)
{
    // Connect mediaFileManager signals
    connect(mediaFileManager, &MediaFileManager::fileOpened, this, [this](const QString &filePath) {
        emit updateWindowTitle(QString("Legilimens - %1").arg(filePath));
    });

    connect(mediaFileManager, &MediaFileManager::fileClosed, this, [this]() {
        emit updateWindowTitle("Legilimens");
    });

    connect(mediaFileManager, &MediaFileManager::error, this, &Controller::error);
}

Controller::~Controller()
{
}

void Controller::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Open File"), "", tr("All Files (*)"));
    if (!fileName.isEmpty()) {
        openFile(fileName);
    }
}

bool Controller::openFile(const QString &filePath)
{
    if (filePath.isEmpty()) {
        emit error("No file selected");
        return false;
    }
    return mediaFileManager->openFile(filePath);
}

void Controller::closeFile()
{
    mediaFileManager->closeFile();
}

void Controller::play()
{
    // TODO: Implement playback logic
}

void Controller::pause()
{
    // TODO: Implement pause logic
}

void Controller::stop()
{
    // TODO: Implement stop logic
}

void Controller::resume()
{
    // TODO: Implement resume logic
} 