#ifndef CINFOSET_H
#define CINFOSET_H

#include <QDialog>

namespace Ui {
class cInfoSet;
}

class cInfoSet : public QDialog
{
    Q_OBJECT

public:
    explicit cInfoSet(QWidget *parent = nullptr);
    ~cInfoSet();

private slots:
    void portEntered();
    void okClicked();

private:
    Ui::cInfoSet *ui;
};

#endif // CINFOSET_H
