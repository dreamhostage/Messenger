#ifndef INFOSET_H
#define INFOSET_H

#include <QDialog>

namespace Ui {
class InfoSet;
}

class InfoSet : public QDialog
{
    Q_OBJECT

public:
    explicit InfoSet(QWidget *parent = nullptr);
    ~InfoSet();

private slots:
    void okClicked();
    void portEntered();


private:
    Ui::InfoSet *ui;
};

#endif // INFOSET_H
