#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "intser.h"

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
    void on_lineEdit_textChanged();
    void on_pushButton_clicked();
    void on_StopBut_clicked();
    void on_checkBox_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    InterceptServer server;
    struct inf
    {
        InterceptServer *server;
        Ui::MainWindow *ui;
        bool *terminated;
    } info;
    bool terminated;
};

#endif // MAINWINDOW_H
