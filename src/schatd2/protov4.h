#ifndef PROTOV4_H
#define PROTOV4_H

#include <QtGui/QWidget>
#include "ui_protov4.h"

class protov4 : public QWidget
{
    Q_OBJECT

public:
    protov4(QWidget *parent = 0);
    ~protov4();

private:
    Ui::protov4Class ui;
};

#endif // PROTOV4_H
