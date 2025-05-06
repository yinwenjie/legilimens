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
    QDockWidget *streamsDock = nullptr;

private slots:
    void onOpenFile();
    void onCloseFile();
    void onAboutUs();
    void onStreams();
};
#endif // MAINWINDOW_H
