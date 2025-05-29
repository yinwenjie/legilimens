#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QString>
#include <QFileInfo>

class Model : public QObject
{
    Q_OBJECT

public:
    explicit Model(QObject *parent = nullptr);
    virtual ~Model();

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

#endif // MODEL_H 