#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    QAction *streamsAction = nullptr;
    QAction *sliceAction = nullptr;
    QAction *hexAction = nullptr;
    QAction *macroblockAction = nullptr;
    QDockWidget *streamsDock = nullptr;
    QDockWidget *sliceDock = nullptr;
    QDockWidget *hexDock = nullptr;
    QDockWidget *macroblockDock = nullptr;

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
};
#endif // MAINWINDOW_H
