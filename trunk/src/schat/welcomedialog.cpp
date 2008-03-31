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
  
  sexBox->addItem(QIcon(":/images/male.png"), tr("Мужской"));
  sexBox->addItem(QIcon(":/images/female.png"), tr("Женский"));
  
  nickEdit->setText(nick);
  nameEdit->setText(name);
  
  if (sex)
    sexBox->setCurrentIndex(1);
  else
    sexBox->setCurrentIndex(0);
}


/** [public]
 *
 */
void WelcomeDialog::setHideWelcome(bool w) { askCheckBox->setChecked(w); }


/** [public]
 *
 */
QString WelcomeDialog::nick() { return nickEdit->text(); }


/** [public]
 *
 */
QString WelcomeDialog::fullName() { return nameEdit->text(); }


/** [public]
 *
 */
bool WelcomeDialog::hideWelcome() { return askCheckBox->isChecked(); }


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
