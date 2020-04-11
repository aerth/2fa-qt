#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // get list of OTP names
    QProcess process;
    process.start("2fa -list");
    process.waitForFinished(-1);
    QString cmd_stdout = process.readAllStandardOutput();
    // ouch. maybe don't have 2fa command.
    if (cmd_stdout.length() == 0) {
       QString cmd_stderr = process.readAllStandardError();
       QMessageBox messageBox;
       messageBox.critical(nullptr,"Error",cmd_stderr);
       exit(111);
    } else {
    // populate combobox
    QStringList items = cmd_stdout.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    ui->comboBox->addItems(items);
    this->currentKey = items[0];
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBox_activated(const QString &arg1)
{
    this->currentKey = arg1;
}

void MainWindow::on_pushButton_released()
{
    // ouch
    if (this->currentKey == nullptr){
        QMessageBox messagebox;
        messagebox.critical(nullptr,"Error", "No key selected");
        return;
    }

    // get OTP
    QProcess process;
    process.start("2fa " + this->currentKey);
    process.waitForFinished(-1); // will wait forever until finished
    QString cmd_stdout = process.readAllStandardOutput();
    if (cmd_stdout.length() == 0) {
       QString cmd_stderr = process.readAllStandardError();
       QMessageBox messageBox;
       messageBox.critical(nullptr,"Error",cmd_stderr);
       exit(111);
    }

    // clipboard "copy"
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(cmd_stdout.trimmed());
    // exit(0); // clears clipboard
    this->lower();
}

void MainWindow::on_actionQuit_triggered()
{
    exit(0);
}
