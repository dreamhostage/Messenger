#ifndef CHATINGROOM_H
#define CHATINGROOM_H

#include <QDialog>
#include "chat.h"

extern wait_for *server;
extern HANDLE hConnect;
extern bool client_session;

namespace Ui {
class ChatingRoom;
}

class ChatingRoom : public QDialog
{
    Q_OBJECT

public:
    explicit ChatingRoom(QString name, QWidget *parent = nullptr, int i = 0, QString ip = "", bool session = false);
    void accrpt();
    QString name;

    ~ChatingRoom();

private slots:
    void on_SendBut_clicked();
    void okEnabled();
    void tChanged();

private:
    Ui::ChatingRoom *ui;
    void ChatingRoom::closeEvent(QCloseEvent *event);
};

#endif // CHATINGROOM_H
