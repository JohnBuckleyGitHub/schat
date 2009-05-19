/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>

#include "abstractprofile.h"
#include "profilewidget.h"
#include "settings.h"

/*!
 * Конструктор класса ProfileWidget.
 */
ProfileWidget::ProfileWidget(QWidget *parent)
  : QWidget(parent),
  m_profile(SimpleSettings->profile())
{
  setAttribute(Qt::WA_DeleteOnClose);

  m_nick = new QLineEdit(m_profile->nick(), this);
  m_nick->setMaxLength(AbstractProfile::MaxNickLength);

  m_name = new QLineEdit(m_profile->fullName(), this);
  m_name->setMaxLength(AbstractProfile::MaxNameLength);

  m_gender = new QComboBox(this);
  m_gender->addItem(QIcon(":/images/male.png"), tr("Мужской"));
  m_gender->addItem(QIcon(":/images/female.png"), tr("Женский"));
  m_gender->setCurrentIndex(m_profile->genderNum());

  connect(m_nick, SIGNAL(textChanged(const QString &)), this, SLOT(validateNick(const QString &)));

  QFormLayout *mainLay = new QFormLayout(this);
  mainLay->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  mainLay->setMargin(0);
  mainLay->setSpacing(5);
  mainLay->addRow(tr("&Ник:"), m_nick);
  mainLay->addRow(tr("&ФИO:"), m_name);
  mainLay->addRow(tr("&Пол:"), m_gender);
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
