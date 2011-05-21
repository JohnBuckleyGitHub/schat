/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#include <QComboBox>
#include <QCompleter>
#include <QEvent>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
//#include <QDebug>

#include "abstractprofile.h"
#include "profilewidget.h"
#include "settings.h"
#include "widget/nickedit.h"

/*!
 * Конструктор класса ProfileWidget.
 */
ProfileWidget::ProfileWidget(bool compactGender, QWidget *parent)
  : TranslateWidget(parent),
  m_profile(SimpleSettings->profile()),
  m_compactGenderWidget(compactGender),
  m_maxRecentItems(SimpleSettings->getInt("Profile/MaxRecentItems"))
{
  setAttribute(Qt::WA_DeleteOnClose);

  m_nickEdit = new NickEdit(this, m_compactGenderWidget ? NickEdit::GenderButton : NickEdit::NoOptions);

  m_name = new QLineEdit(this);
  m_name->setMaxLength(AbstractProfile::MaxNameLength);
  if (m_maxRecentItems) {
    m_name->setCompleter(new QCompleter(SimpleSettings->getList("Profile/RecentRealNames"), m_name));
    m_name->completer()->setCaseSensitivity(Qt::CaseInsensitive);
  }

  if (!m_compactGenderWidget) {
    m_gender = new QComboBox(this);
    m_gender->addItem(QIcon(":/images/male.png"),   "");
    m_gender->addItem(QIcon(":/images/female.png"), "");
  }

  connect(m_nickEdit, SIGNAL(validNick(bool)), SIGNAL(validNick(bool)));

  m_mainLay = new QFormLayout(this);
  m_mainLay->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  m_mainLay->setMargin(0);
  m_mainLay->setSpacing(5);
  m_mainLay->addRow(" ", m_nickEdit);
  m_mainLay->addRow(" ", m_name);

  if (!m_compactGenderWidget)
    m_mainLay->addRow(" ", m_gender);

  reload();
  retranslateUi();
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


void ProfileWidget::reload()
{
  m_nickEdit->reload();

  m_name->setText(m_profile->fullName());
  if (!m_compactGenderWidget) {
    m_gender->setCurrentIndex(m_profile->genderNum());
  }
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


void ProfileWidget::retranslateUi()
{
  static_cast<QLabel *>(m_mainLay->labelForField(m_nickEdit))->setText(tr("&Nick:"));
  static_cast<QLabel *>(m_mainLay->labelForField(m_name))->setText(tr("N&ame:"));

  /// \bug При изменении языка не происходит изменения размера виджета выбора пола.
  if (!m_compactGenderWidget) {
    m_gender->setItemText(0, tr("Male"));
    m_gender->setItemText(1, tr("Female"));
    static_cast<QLabel *>(m_mainLay->labelForField(m_gender))->setText(tr("&Sex:"));
  }
}
