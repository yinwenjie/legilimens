#include "controller.h"
#include <QFileDialog>
#include <QMessageBox>

Controller::Controller(Model *model, QObject *parent)
    : QObject(parent)
    , model(model)
{
    // Connect model signals
    connect(model, &Model::fileOpened, this, [this](const QString &filePath) {
        emit updateWindowTitle(QString("Legilimens - %1").arg(filePath));
    });

    connect(model, &Model::fileClosed, this, [this]() {
        emit updateWindowTitle("Legilimens");
    });

    connect(model, &Model::error, this, &Controller::showError);
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