#include "controller.h"
#include <QFileDialog>
#include <QMessageBox>

Controller::Controller(MediaFileManager *model, QObject *parent)
    : QObject(parent)
    , model(model)
{
    // Connect model signals
    connect(model, &MediaFileManager::fileOpened, this, [this](const QString &filePath) {
        emit updateWindowTitle(QString("Legilimens - %1").arg(filePath));
    });

    connect(model, &MediaFileManager::fileClosed, this, [this]() {
        emit updateWindowTitle("Legilimens");
    });

    connect(model, &MediaFileManager::error, this, &Controller::error);
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
    return model->openFile(filePath);
}

void Controller::closeFile()
{
    model->closeFile();
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