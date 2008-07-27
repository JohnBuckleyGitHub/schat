/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>

#include "abstractprofile.h"
#include "profilewidget.h"


ProfileWidget::ProfileWidget(AbstractProfile *p, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  m_profile = p;
  
  m_nickLabel   = new QLabel(tr("Ник:"), this);
  m_nameLabel   = new QLabel(tr("ФИO:"), this);
  m_genderLabel = new QLabel(tr("Пол:"), this);
  m_nick        = new QLineEdit(m_profile->nick(), this);
  m_name        = new QLineEdit(m_profile->fullName(), this);
  m_gender      = new QComboBox(this);
  QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  
  m_gender->addItem(QIcon(":/images/male.png"), tr("Мужской"));
  m_gender->addItem(QIcon(":/images/female.png"), tr("Женский"));
  m_gender->setCurrentIndex(m_profile->genderNum());
  
  m_nick->setMaxLength(AbstractProfile::MaxNickLength);
  m_name->setMaxLength(AbstractProfile::MaxNameLength);
  
  connect(m_nick, SIGNAL(textChanged(const QString &)), this, SLOT(validateNick(const QString &)));
  
  QGridLayout *mainLayout = new QGridLayout(this);
  mainLayout->addWidget(m_nickLabel, 0, 0);
  mainLayout->addWidget(m_nick, 0, 1, 1, 2);
  mainLayout->addWidget(m_nameLabel, 1, 0);
  mainLayout->addWidget(m_name, 1, 1, 1, 2);
  mainLayout->addWidget(m_genderLabel, 2, 0);
  mainLayout->addWidget(m_gender, 2, 1);
  mainLayout->addItem(spacer, 2, 2);
  mainLayout->setMargin(0);
}


/** [public]
 * 
 */
void ProfileWidget::reset()
{
  m_nick->setText(QDir::home().dirName());
  m_name->setText("");
  m_gender->setCurrentIndex(0);
}


/** [public]
 * 
 */
void ProfileWidget::save()
{
  m_modifiled = false;
  
  if (m_profile->nick() != m_nick->text()) {
    m_profile->setNick(m_nick->text());
    m_modifiled = true;
  }
  
  if (m_profile->fullName() != m_name->text()) {
    m_profile->setFullName(m_name->text());
    m_modifiled = true;
  }
    
  if (m_profile->genderNum() != quint8(m_gender->currentIndex())) {
    m_profile->setGender(quint8(m_gender->currentIndex()));
    m_modifiled = true;
  }
}


/** [private slots]
 * 
 */
void ProfileWidget::validateNick(const QString &text)
{
  QPalette p = m_nick->palette();
  bool b     = AbstractProfile::isValidNick(text);
  
  if (b)
    p.setColor(QPalette::Active, QPalette::Base, Qt::white);
  else
    p.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
  
  emit validNick(b);
  m_nick->setPalette(p);
}
