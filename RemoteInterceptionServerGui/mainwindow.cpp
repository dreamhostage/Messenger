#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegExpValidator>
#include <Windows.h>

DWORD WINAPI serverEng(PVOID p)
{
    Ui::MainWindow * ui = static_cast<Ui::MainWindow *>(p);

    return 0;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setDisabled(true);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::on_lineEdit_textChanged);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
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
    HANDLE hThread = CreateThread(NULL, 0, serverEng, &ui, 0, 0);
    CloseHandle(hThread);
}
