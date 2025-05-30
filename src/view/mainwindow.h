#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QAction>
#include <QDockWidget>
#include "controller/controller.h"
#include "view/widgets/sequencewidgetmanager.h"
#include "view/widgets/streamswidgetmanager.h"
#include "view/widgets/slicewidgetmanager.h"
#include "view/widgets/hexwidgetmanager.h"
#include "view/widgets/macroblockwidgetmanager.h"

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
    void showEvent(QShowEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    Ui::MainWindow *ui;
    Controller *controller;

    // Specialized widget managers
    SequenceWidgetManager* sequenceManager = nullptr;
    StreamsWidgetManager* streamsManager = nullptr;
    SliceWidgetManager* sliceManager = nullptr;
    HexWidgetManager* hexManager = nullptr;
    MacroblockWidgetManager* macroblockManager = nullptr;

    // Helper methods
    void setupDockAreaPriorities();
    void createMenus();
    void createFileMenu();
    void createViewMenu();
    void createPlaybackMenu();
    void createHelpMenu();
    void setupDockWidgets();
    void setupConnections();
    void setupInitialWidgetSizes();

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
