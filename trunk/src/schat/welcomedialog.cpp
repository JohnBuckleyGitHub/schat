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
WelcomeDialog::WelcomeDialog(const QString &nick, const QString &name, quint8 sex, QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  setupUi(this);
  
  serverLabel->setVisible(false);
  serverEdit->setVisible(false);
  
  sexBox->addItem(QIcon(":/images/male.png"), tr("Мужской"));
  sexBox->addItem(QIcon(":/images/female.png"), tr("Женский"));
  
  nickEdit->setText(nick);
  nameEdit->setText(name);
  
  if (sex)
    sexBox->setCurrentIndex(1);
  else
    sexBox->setCurrentIndex(0);
  
  connect(moreButton, SIGNAL(toggled(bool)), this, SLOT(changeIcon(bool)));
}


/** [public]
 *
 */
quint8 WelcomeDialog::sex()
{
  if (sexBox->currentIndex() == 0)
    return 0;
  else
    return 1;
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
