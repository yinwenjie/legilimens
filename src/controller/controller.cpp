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
        emit clearAllWidgets();
    });

    connect(model, &MediaFileManager::error, this, &Controller::error);

    // Forward stream information updates using queued connection to prevent destruction issues
    connect(model, &MediaFileManager::streamsInfoUpdated, this, &Controller::streamInfoUpdated, Qt::QueuedConnection);

    // Forward parser thread signals
    connect(model, &MediaFileManager::slicesParsed, this, &Controller::slicesParsed, Qt::QueuedConnection);
    connect(model, &MediaFileManager::parsingProgress, this, &Controller::parsingProgress, Qt::QueuedConnection);
    connect(model, &MediaFileManager::parsingFinished, this, &Controller::parsingFinished, Qt::QueuedConnection);
}

Controller::~Controller()
{
    // Stop any ongoing parsing
    if (model) {
        model->stopParsing();
    }
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

void Controller::startParsing()
{
    if (model) {
        model->startParsing();
    }
}

void Controller::stopParsing()
{
    if (model) {
        model->stopParsing();
    }
}

bool Controller::isParsing() const
{
    return model ? model->isParsing() : false;
}

void Controller::setAutoParsingEnabled(bool enabled)
{
    if (model) {
        model->setAutoParsingEnabled(enabled);
    }
}

bool Controller::isAutoParsingEnabled() const
{
    return model ? model->isAutoParsingEnabled() : false;
} 