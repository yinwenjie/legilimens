#ifndef MEDIAFILEMANAGER_H
#define MEDIAFILEMANAGER_H

#include <QObject>
#include <QString>
#include <QFileInfo>

class MediaFileManager : public QObject
{
    Q_OBJECT

public:
    explicit MediaFileManager(QObject *parent = nullptr);
    virtual ~MediaFileManager();

    // File operations
    bool openFile(const QString &filePath);
    void closeFile();
    QString getCurrentFilePath() const;
    qint64 getFileSize() const;

signals:
    void fileOpened(const QString &filePath);
    void fileClosed();
    void error(const QString &message);

private:
    QString currentFilePath;
    qint64 fileSize;
};

#endif // MEDIAFILEMANAGER_H 