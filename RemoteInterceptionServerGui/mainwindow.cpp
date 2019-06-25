#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "boost/asio.hpp"
#include <QRegExpValidator>
#include <Windows.h>
#include "RemoteInterceptionServerLib.h"
#include <QString>
#include <QScrollBar>

DWORD WINAPI serverEng(PVOID p)
{
    Ui::MainWindow *ui = static_cast<Ui::MainWindow*>(p);

    server engine(ui->lineEdit->text().toInt());

    while(true)
    {
        ui->textBrowser->append(QString::fromStdString(engine.loop()));
        ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
    }

    return 0;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setDisabled(true);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::on_lineEdit_textChanged);
    connect(ui->pushButton, &QPushButton::click, this, &MainWindow::on_pushButton_clicked);
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
    ui->textBrowser->append("<b>Waiting for connections...\n\n<\b>");
    ui->pushButton->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    HANDLE hThread = CreateThread(nullptr, 0, serverEng, ui, 0, 0);
    CloseHandle(hThread);
}
