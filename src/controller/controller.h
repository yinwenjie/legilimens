#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>
#include "model/model.h"

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(Model *model, QObject *parent = nullptr);
    virtual ~Controller();

    // File operations
    void openFile();
    void closeFile();

    // Playback operations
    void play();
    void pause();
    void stop();
    void resume();

signals:
    void updateWindowTitle(const QString &title);
    void showError(const QString &message);

private:
    Model *model;
};

#endif // CONTROLLER_H 