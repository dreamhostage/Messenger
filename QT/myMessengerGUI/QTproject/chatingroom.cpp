#include "chatingroom.h"
#include "ui_chatingroom.h"
#include "chat.h"
#include <Windows.h>
#include <QMessageBox>
#include <QScrollBar>
#include "infoset.h"
#include <iostream>

wait_for *server;
HANDLE hConnect;
bool client_session;

DWORD WINAPI listen(PVOID param)
{
   Ui::ChatingRoom *ui = static_cast<Ui::ChatingRoom*>(param);

    again:

   ui->textBrowser->append("<b>Waiting for connection...\n<\b>");

   if(!client_session)
   {
       if(!server->accept())
       {
           ui->textBrowser->setTextColor(Qt::red);
           ui->textBrowser->append("Somebody entered...");
           ui->textBrowser->setTextColor(Qt::black);
       }
       else
       {
           ui->textBrowser->append("Connection ERROR!");
           return 0;
       }
   }
   else
   {
       if(!server->connect(*server->ip, *server->port))
       {

       }
       else
       {
           ui->textBrowser->append("Connection ERROR!");
           return 0;
       }
   }

    QString message;
    while(message != "**********EXIT**********")
    {
       ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
        message = QString::fromStdString(server->read());
        if(message == "**********EXIT**********")
                break;
        if(message == "**********123456789**********")
                ui->textBrowser->append(QString::fromStdString(*server->connectedName) + " connected");
        if(message != "**********123456789**********")
            ui->textBrowser->append("[" + QString::fromStdString(*server->connectedName) + "]: " + message);
    }

   ui->textBrowser->setTextColor(Qt::red);
   ui->textBrowser->append(QString::fromStdString(*server->connectedName) + " LEFT the chat!\n");
   ui->textBrowser->setTextColor(Qt::black);
   *server->connection = FALSE;

   server->reset();

    goto again;

    return 0;
}



ChatingRoom::ChatingRoom(QString name, QWidget *parent, int port, QString ip, bool session) :
    QDialog(parent),
    ui(new Ui::ChatingRoom)
{
    ui->setupUi(this);
    ui->SendBut->setDisabled(true);
    ChatingRoom::setLayout(ui->mLayout);

    server = new wait_for(port, session);
    *server->yourName = name.toStdString();
    *server->ip = ip.toStdString();
    *server->port = port;
    client_session = session;

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &ChatingRoom::okEnabled);
    connect(ui->textBrowser, &QTextBrowser::textChanged, this, &ChatingRoom::tChanged);

    hConnect = CreateThread(nullptr, 0, listen, ui, 0, nullptr);
}

ChatingRoom::~ChatingRoom()
{
    delete ui;

}

void ChatingRoom::on_SendBut_clicked()
{
    if(*server->connection)
    {
    if(!ui->lineEdit->text().isEmpty())
        {
           ui->textBrowser->append("<em>" + ui->lineEdit->text());
           server->write(ui->lineEdit->text().toStdString());
           ui->lineEdit->clear();
        }
    }
}

void ChatingRoom::okEnabled()
{
    if(*server->connection)
        ui->SendBut->setEnabled(true);
}

void ChatingRoom::closeEvent(QCloseEvent *event1)
{
    TerminateThread(hConnect, 0);
    WaitForSingleObject(hConnect, INFINITE);
    delete server;
}

void ChatingRoom::tChanged()
{
    ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
}
