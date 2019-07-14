#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chatingroom.h"
#include <Windows.h>
#include <QMessageBox>
#include "infoset.h"
#include "cinfoset.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    MainWindow::setVisible(false);
    InfoSet *wnd = new InfoSet(this);
    wnd->show();
    wnd->exec();
    MainWindow::setVisible(true);
}

void MainWindow::on_pushButton_2_clicked()
{
    MainWindow::setVisible(false);
    cInfoSet *wnd = new cInfoSet(this);
    wnd->show();
    wnd->exec();
    MainWindow::setVisible(true);
}
