#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QAction>
#include <QDockWidget>
#include "common/dockwidgetmanager.h"
#include "controller/controller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::MainWindow *ui;
    Controller *controller;

    // Replace individual dock and action members with DockWidgetManager instances
    DockWidgetManager* sequenceManager = nullptr;
    DockWidgetManager* streamsManager = nullptr;
    DockWidgetManager* sliceManager = nullptr;
    DockWidgetManager* hexManager = nullptr;
    DockWidgetManager* macroblockManager = nullptr;

    // Helper methods
    void setupDockAreaPriorities();
    void createMenus();
    void createFileMenu();
    void createViewMenu();
    void createPlaybackMenu();
    void createHelpMenu();
    void setupDockWidgets();
    void setupConnections();

private slots:
    void onOpenFile();
    void onCloseFile();
    void onAboutUs();
    void onStreams();
    void onSlice();
    void onHex();
    void onMacroblock();
    void onSequence();

    void onPlay();
    void onPause();
    void onStop();
    void onResume();

    void updateWindowTitle(const QString &title);
    void showError(const QString &message);
};
#endif // MAINWINDOW_H
