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
#include "widget/nickedit.h"

/*!
 * Конструктор класса ProfileWidget.
 */
ProfileWidget::ProfileWidget(bool compactGender, QWidget *parent)
  : QWidget(parent),
  m_profile(SimpleSettings->profile()),
  m_compactGenderWidget(compactGender),
  m_maxRecentItems(SimpleSettings->getInt("Profile/MaxRecentItems"))
{
  setAttribute(Qt::WA_DeleteOnClose);

  m_nickEdit = new NickEdit(this, m_compactGenderWidget ? NickEdit::GenderButton : NickEdit::NoOptions);

  m_name = new QLineEdit(m_profile->fullName(), this);
  m_name->setMaxLength(AbstractProfile::MaxNameLength);
  if (m_maxRecentItems) {
    m_name->setCompleter(new QCompleter(SimpleSettings->getList("Profile/RecentRealNames"), m_name));
    m_name->completer()->setCaseSensitivity(Qt::CaseInsensitive);
  }

  if (!m_compactGenderWidget) {
    m_gender = new QComboBox(this);
    m_gender->addItem(QIcon(":/images/male.png"),   tr("Мужской"));
    m_gender->addItem(QIcon(":/images/female.png"), tr("Женский"));
    m_gender->setCurrentIndex(m_profile->genderNum());
  }

  connect(m_nickEdit, SIGNAL(validNick(bool)), SIGNAL(validNick(bool)));

  QFormLayout *mainLay = new QFormLayout(this);
  mainLay->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  mainLay->setMargin(0);
  mainLay->setSpacing(5);
  mainLay->addRow(tr("&Ник:"), m_nickEdit);
  mainLay->addRow(tr("&ФИO:"), m_name);

  if (!m_compactGenderWidget)
    mainLay->addRow(tr("&Пол:"), m_gender);
}


/*!
 * Сохранение настроек.
 */
int ProfileWidget::save()
{
  if (!m_compactGenderWidget)
    m_nickEdit->setGender(m_gender->currentIndex());

  int modified = m_nickEdit->save(false);

  if (m_profile->fullName() != m_name->text()) {
    m_profile->setFullName(m_name->text());
    modified++;
    if (m_maxRecentItems)
      NickEdit::modifyRecentList("Profile/RecentRealNames", m_profile->fullName());
  }

  if (modified)
    SimpleSettings->notify(Settings::ProfileSettingsChanged);

  return modified;
}


/*!
 * Сброс введённых данных на стандартные значения.
 */
void ProfileWidget::reset()
{
  m_nickEdit->reset();
  m_name->setText("");

  if (!m_compactGenderWidget)
    m_gender->setCurrentIndex(0);
}
