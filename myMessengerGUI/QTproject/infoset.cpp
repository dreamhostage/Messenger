#include "infoset.h"
#include "ui_infoset.h"
#include <QIntValidator>
#include "chatingroom.h"

InfoSet::InfoSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoSet)
{
    ui->setupUi(this);
    InfoSet::setVisible(true);
    InfoSet::setLayout(ui->verticalLayout);
    ui->lineEdit_2->setValidator( new QIntValidator(0, 9999, this) );
    ui->okB->setDisabled(true);
    connect(ui->okB, &QPushButton::clicked, this, &InfoSet::okClicked);
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &InfoSet::portEntered);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &InfoSet::portEntered);
}

InfoSet::~InfoSet()
{
    delete ui;
}

void InfoSet::okClicked()
{
    ChatingRoom *chRoom = new ChatingRoom(ui->lineEdit->text(), this, ui->lineEdit_2->text().toInt(), " ", false);
    chRoom->show();
    InfoSet::setVisible(false);
    chRoom->exec();
    InfoSet::close();
}

void InfoSet::portEntered()
{
    if(!ui->lineEdit->text().isEmpty() && !ui->lineEdit_2->text().isEmpty())
        ui->okB->setEnabled(true);
    else
        ui->okB->setDisabled(true);

}
