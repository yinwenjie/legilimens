#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QAction>
#include <QDockWidget>
#include "dockwidgetmanager.h"

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
};
#endif // MAINWINDOW_H
