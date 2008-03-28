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
WelcomeDialog::WelcomeDialog(QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  setupUi(this);
  
  nickEdit->setText(QDir::home().dirName());
  sexBox->addItem(QIcon(":/images/male.png"), tr("Мужской"));
  sexBox->addItem(QIcon(":/images/female.png"), tr("Женский"));
}


/** [public]
 *
 */
QString WelcomeDialog::nick()
{
  return nickEdit->text();
}


/** [public]
 *
 */
QString WelcomeDialog::fullName()
{
  return nameEdit->text();
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

