#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDialog>
#include <intser.h>

namespace Ui {
class message;
}

class message : public QDialog
{
    Q_OBJECT

public:
    explicit message(InterceptServer &server ,QWidget *parent = nullptr);
    ~message();

private slots:


    void on_plainTextEdit_textChanged();

    void on_pushButton_clicked();

private:
    Ui::message *ui;
    InterceptServer *server;
};

#endif // MESSAGE_H
