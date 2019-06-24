#include "cinfoset.h"
#include "ui_cinfoset.h"
#include <QIntValidator>
#include "chatingroom.h"

cInfoSet::cInfoSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cInfoSet)
{
    ui->setupUi(this);
    cInfoSet::setLayout(ui->verticalLayout);
    ui->okB->setDisabled(true);
    ui->lineEdit_2->setValidator( new QIntValidator(0, 9999, this) );
    QRegExp exp("[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}");
    ui->lineEdit_3->setValidator(new QRegExpValidator(exp, this));

    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &cInfoSet::portEntered);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &cInfoSet::portEntered);
    connect(ui->lineEdit_3, &QLineEdit::textChanged, this, &cInfoSet::portEntered);
    connect(ui->okB, &QPushButton::clicked, this, &cInfoSet::okClicked);
}

cInfoSet::~cInfoSet()
{
    delete ui;
}

void cInfoSet::portEntered()
{
    if(!ui->lineEdit->text().isEmpty() && !ui->lineEdit_2->text().isEmpty() && !ui->lineEdit_3->text().isEmpty())
        ui->okB->setEnabled(true);
    else
        ui->okB->setDisabled(true);
}

void cInfoSet::okClicked()
{
    ChatingRoom *chRoom = new ChatingRoom(ui->lineEdit->text(), this, ui->lineEdit_2->text().toInt(), ui->lineEdit_3->text(), true);
    chRoom->show();
    cInfoSet::setVisible(false);
    chRoom->exec();
    cInfoSet::close();
}
