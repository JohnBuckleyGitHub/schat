/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "profile.h"
#include "profilewidget.h"


ProfileWidget::ProfileWidget(Profile *p, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  profile = p;
  
  nickLabel = new QLabel(tr("Ник:"), this);
  nameLabel = new QLabel(tr("ФИO:"), this);
  sexLabel  = new QLabel(tr("Пол:"), this);
  nickEdit  = new QLineEdit(profile->nick(), this);
  nameEdit  = new QLineEdit(profile->fullName(), this);
  sexBox    = new QComboBox(this);
  QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  
  sexBox->addItem(QIcon(":/images/male.png"), tr("Мужской"));
  sexBox->addItem(QIcon(":/images/female.png"), tr("Женский"));  
  sexBox->setCurrentIndex(profile->sex());
  nickEdit->setMaxLength(64);
  nameEdit->setMaxLength(255);
  
  connect(nickEdit, SIGNAL(textChanged(const QString &)), this, SLOT(validateNick(const QString &)));
  
  QGridLayout *mainLayout = new QGridLayout(this);
  mainLayout->addWidget(nickLabel, 0, 0);
  mainLayout->addWidget(nickEdit, 0, 1, 1, 2);
  mainLayout->addWidget(nameLabel, 1, 0);
  mainLayout->addWidget(nameEdit, 1, 1, 1, 2);
  mainLayout->addWidget(sexLabel, 2, 0);
  mainLayout->addWidget(sexBox, 2, 1);
  mainLayout->addItem(spacer, 2, 2);
  mainLayout->setMargin(0);
}


/** [public]
 * 
 */
void ProfileWidget::reset()
{
  nickEdit->setText(QDir::home().dirName());
  nameEdit->setText("");
  sexBox->setCurrentIndex(0);
}


/** [public]
 * 
 */
void ProfileWidget::save()
{
  modifiled = false;
  
  if (profile->nick() != nickEdit->text()) {
    profile->setNick(nickEdit->text());
    modifiled = true;
  }
  
  if (profile->fullName() != nameEdit->text()) {
    profile->setFullName(nameEdit->text());
    modifiled = true;
  }
    
  if (profile->sex() != quint8(sexBox->currentIndex())) {
    profile->setSex(quint8(sexBox->currentIndex()));
    modifiled = true;
  }
}


/** [private slots]
 * 
 */
void ProfileWidget::validateNick(const QString &text)
{
  QPalette p = nickEdit->palette();
  bool b     = Profile::isValidNick(text);
  
  if (b) {
    p.setColor(QPalette::Active, QPalette::Base, Qt::white);
  }
  else {
    p.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
  }
  
  emit validNick(b);
  nickEdit->setPalette(p);
}
