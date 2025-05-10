#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

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

private:
    Ui::MainWindow *ui;

    QAction *sequenceAction = nullptr;
    QAction *streamsAction = nullptr;
    QAction *sliceAction = nullptr;
    QAction *hexAction = nullptr;
    QAction *macroblockAction = nullptr;

    QDockWidget *sequenceDock = nullptr;
    QDockWidget *streamsDock = nullptr;
    QDockWidget *sliceDock = nullptr;
    QDockWidget *hexDock = nullptr;
    QDockWidget *macroblockDock = nullptr;

    // Map to store dock widgets and their corresponding actions
    QMap<QDockWidget*, QAction*> dockActionMap;

    // Helper method to create dock widgets
    QDockWidget* createDockWidget(const QString& title, QAction* action, Qt::DockWidgetArea area);

private slots:
    void onOpenFile();
    void onCloseFile();
    void onAboutUs();
    void onStreams();
    void onSlice();
    void onHex();
    void onMacroblock();

    void onPlay();
    void onPause();
    void onStop();
    void onResume();
    void onSequence();
};
#endif // MAINWINDOW_H
