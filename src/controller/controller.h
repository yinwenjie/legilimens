#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>
#include "model/mediafilemanager.h"

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(MediaFileManager *model, QObject *parent = nullptr);
    ~Controller();

    // File operations
    void openFile();
    bool openFile(const QString &filePath);
    void closeFile();

    // Playback operations
    void play();
    void pause();
    void stop();
    void resume();

signals:
    void updateWindowTitle(const QString &title);
    void error(const QString &message);

private:
    MediaFileManager *mediaFileManager;
};

#endif // CONTROLLER_H 