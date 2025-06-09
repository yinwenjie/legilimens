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
    
    // Parser thread operations
    void startParsing();
    void stopParsing();
    bool isParsing() const;
    
    // Auto-parsing control
    void setAutoParsingEnabled(bool enabled);
    bool isAutoParsingEnabled() const;
    
    // Stream information access
    MediaFileManager* getMediaFileManager() const { return model; }

signals:
    void updateWindowTitle(const QString &title);
    void error(const QString &message);
    void streamInfoUpdated(const QList<VideoStreamInfo> &videoStreams, const QList<AudioStreamInfo> &audioStreams);
    void slicesParsed(const QList<SliceInfo> &slices);
    void parsingProgress(int percentage);
    void parsingFinished();

private:
    MediaFileManager *model;
};

#endif // CONTROLLER_H 