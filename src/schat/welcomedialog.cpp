/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "welcomedialog.h"
#include "schatwindow.h"


/** [public]
 * Конструктор
 */
WelcomeDialog::WelcomeDialog(Profile *p, QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  setupUi(this);
  
  profile = p;
  
  serverLabel->setVisible(false);
  serverEdit->setVisible(false);  
  sexBox->addItem(QIcon(":/images/male.png"), tr("Мужской"));
  sexBox->addItem(QIcon(":/images/female.png"), tr("Женский"));  
  nickEdit->setText(profile->nick());
  nameEdit->setText(profile->fullName());
  sexBox->setCurrentIndex(profile->sex());
  
  connect(moreButton, SIGNAL(toggled(bool)), this, SLOT(changeIcon(bool)));
}


/** [public]
 *
 */
void WelcomeDialog::accept()
{
  profile->setNick(nickEdit->text());
  profile->setFullName(nameEdit->text());
  profile->setSex(quint8(sexBox->currentIndex()));  
  
  QDialog::accept();
}


/** [private slots]
 *
 */
void WelcomeDialog::changeIcon(bool s)
{
  if (s)
    moreButton->setIcon(QIcon(":/images/up.png"));
  else
    moreButton->setIcon(QIcon(":/images/down.png"));
}
