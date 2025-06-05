#include "mediafilemanager.h"
#include <QFileInfo>
#include <QDebug>

MediaFileManager::MediaFileManager(QObject *parent)
    : QObject(parent)
    , fileSize(0)
{
}

MediaFileManager::~MediaFileManager()
{
    closeFile();
}

bool MediaFileManager::openFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        emit error("File does not exist: " + filePath);
        return false;
    }

    currentFilePath = filePath;
    fileSize = fileInfo.size();
    
    // Add logging information
    qDebug() << "File opened successfully:";
    qDebug() << "  Name:" << fileInfo.fileName();
    qDebug() << "  Size:" << fileSize << "bytes";
    
    emit fileOpened(filePath);
    return true;
}

void MediaFileManager::closeFile()
{
    if (!currentFilePath.isEmpty()) {
        currentFilePath.clear();
        fileSize = 0;
        emit fileClosed();
    }
}

QString MediaFileManager::getCurrentFilePath() const
{
    return currentFilePath;
}

qint64 MediaFileManager::getFileSize() const
{
    return fileSize;
} 