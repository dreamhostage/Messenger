#include "message.h"
#include "ui_message.h"
#include <QTextEdit>

message::message(InterceptServer &serv,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::message)
{
    ui->setupUi(this);
    ui->pushButton->setDisabled(true);
    server = &serv;
}

message::~message()
{
    delete ui;
}



void message::on_plainTextEdit_textChanged()
{
    if(!ui->plainTextEdit->toPlainText().isEmpty())
        ui->pushButton->setEnabled(true);
    else {
        ui->pushButton->setEnabled(false);
    }
}

void message::on_pushButton_clicked()
{
    server->setIntercepting(ui->plainTextEdit->toPlainText().toStdString());
    message::close();
}
