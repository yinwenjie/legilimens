#include "model.h"
#include <QFile>
#include <QDebug>

Model::Model(QObject *parent)
    : QObject(parent)
    , fileSize(0)
{
}

Model::~Model()
{
    closeFile();
}

bool Model::openFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        emit error("File does not exist");
        return false;
    }

    currentFilePath = filePath;
    fileSize = fileInfo.size();
    emit fileOpened(filePath);
    return true;
}

void Model::closeFile()
{
    if (!currentFilePath.isEmpty()) {
        currentFilePath.clear();
        fileSize = 0;
        emit fileClosed();
    }
}

QString Model::getCurrentFilePath() const
{
    return currentFilePath;
}

qint64 Model::getFileSize() const
{
    return fileSize;
} 