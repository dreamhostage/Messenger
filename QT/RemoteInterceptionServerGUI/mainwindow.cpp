#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "intser.h"
#include "message.h"
#include <QRegExpValidator>
#include <Windows.h>
#include <QString>
#include <QScrollBar>
#include <QScrollBar>
#include <QMessageBox>

HANDLE hThread;

struct inf
{
    InterceptServer *server;
    Ui::MainWindow *ui;
    bool *terminated;
};

DWORD WINAPI serverEng(PVOID p)
{
    inf* info = static_cast<inf*>(p);
    QString message;
    while(!*info->terminated)
    {
        message = QString::fromStdString(info->server->startWork());
        if(!*info->terminated)
            info->ui->textBrowser->append(message);
        info->ui->textBrowser->verticalScrollBar()->setValue(info->ui->textBrowser->verticalScrollBar()->maximum());

    }
    return 0;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    info.ui = ui;
    info.server = &server;
    info.terminated = &terminated;
    ui->setupUi(this);
    ui->pushButton->setDisabled(true);
    ui->textBrowser->setDisabled(true);
    ui->StopBut->setDisabled(true);
    ui->checkBox->setDisabled(true);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::on_lineEdit_textChanged);
    connect(ui->pushButton, &QPushButton::click, this, &MainWindow::on_pushButton_clicked);
    connect(ui->StopBut, &QPushButton::click, this, &MainWindow::on_StopBut_clicked);

    QRegExp exp("[0-9]{1,4}");
    ui->lineEdit->setValidator(new QRegExpValidator(exp, this));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_textChanged()
{
    if(!ui->lineEdit->text().isEmpty())
        ui->pushButton->setEnabled(true);
    else
        ui->pushButton->setEnabled(false);
}

void MainWindow::on_pushButton_clicked()
{
    ui->textBrowser->append("<b>Waiting for intercepted messages...\n\n<\b>");
    ui->pushButton->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->textBrowser->setEnabled(true);
    ui->StopBut->setEnabled(true);
    ui->checkBox->setEnabled(true);
    terminated = false;
    server.setPort(ui->lineEdit->text().toInt());
    hThread = CreateThread(nullptr, 0, serverEng, &info, 0, 0);
    //CloseHandle(hThread);
}

void MainWindow::on_StopBut_clicked()
{
    //TerminateThread(hThread, 0);
    ui->StopBut->setDisabled(true);
    ui->pushButton->setEnabled(true);
    ui->textBrowser->clear();
    ui->textBrowser->setDisabled(true);
    ui->lineEdit->setEnabled(true);
    ui->checkBox->setDisabled(true);
    terminated = true;
    server.clear();
}

void MainWindow::on_checkBox_clicked(bool checked)
{
    if(checked)
    {
        message mes(server);
        mes.show();
        ui->checkBox->setDisabled(true);
        ui->StopBut->setDisabled(true);
        mes.exec();
        ui->checkBox->setEnabled(true);
        ui->StopBut->setEnabled(true);
    }
    else {
        server.unsetIntercepting();
    }
}
