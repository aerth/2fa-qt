#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QMessageBox>
#include <QClipboard>
#include <QInputDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_comboBox_activated(const QString &arg1);
    void on_pushButton_released();
    void on_actionQuit_triggered();
    void on_actionAdd_New_triggered();
    void repopulate();
    void on_actionAbout_triggered();

private:
    QString currentKey = nullptr;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
