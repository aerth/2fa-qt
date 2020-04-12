#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->repopulate();
    this->ui->pushButton->setAutoDefault(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::repopulate(void)
{
    // get list of OTP names
    QProcess process;
    process.start("2fa -list");
    process.waitForFinished(-1);
    QString cmd_stdout = process.readAllStandardOutput();
    // ouch. maybe don't have 2fa command.
    if (cmd_stdout.length() == 0) {
       QMessageBox::critical(nullptr,"Error", process.readAllStandardError());
       exit(111);
    } else {
    // populate combobox
    QStringList items = cmd_stdout.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    if (ui->comboBox->count() > 0) {
        ui->comboBox->clear();
    }
    ui->comboBox->addItems(items);
    ui->comboBox->setFocus();
    this->currentKey = items[0];
    }
}

void MainWindow::on_comboBox_activated(const QString &arg1)
{
    this->currentKey = arg1;
}

void MainWindow::on_pushButton_released()
{
    // ouch
    if (this->currentKey == nullptr){
        QMessageBox::critical(nullptr,"Error", "No key selected");
        return;
    }

    // get OTP
    QProcess process;
    process.start("2fa " + this->currentKey);
    process.waitForFinished(-1);
    QString cmd_stdout = process.readAllStandardOutput();
    if (cmd_stdout.length() == 0) {
       QString cmd_stderr = process.readAllStandardError();
       QMessageBox::critical(nullptr,"Error",cmd_stderr);
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

void MainWindow::on_actionAdd_New_triggered()
{
    bool ok1, ok2;
    QString text1 = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                        tr("2FA key name:"), QLineEdit::Normal,
                                        "", &ok1);
    if (!ok1) {
        return;
    }
    QString text2 = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                        tr("secret code:"), QLineEdit::Normal,
                                        "", &ok2);
    if (ok1 && !text1.isEmpty() && ok2 && !text2.isEmpty()){
        QProcess process;
        // start the add
        process.start("2fa -add " + text1);
        // write user input to stdin
        process.write(text2.toUtf8()+"\r\n");
        process.waitForFinished(-1);

        // handle non-zero exit code
        QString cmd_stderr = process.readAllStandardError().replace("2fa key for "+text1+":", "");
        cmd_stderr = cmd_stderr.trimmed();
        if (process.exitCode() != 0 || cmd_stderr.length() != 0) {
           QMessageBox::critical(nullptr, "Error " + QString::number(process.exitCode()), "Key not added. " + cmd_stderr);
        } else {
            this->repopulate();
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox messageBox;
    messageBox.setTextFormat(Qt::RichText);
    messageBox.setTextInteractionFlags(Qt::TextBrowserInteraction);
    messageBox.setText("<p><i>Simple QT Frontend for 2fa command.</i></p><p>Source code and README: <a href=\"https://github.com/aerth/2fa-qt\">https://github.com/aerth/2fa-qt</a></p><p>License: MIT</p>");
    messageBox.setWindowTitle("About 2fa-qt");
    messageBox.exec();
}
