/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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
#include <QEvent>
#include <QHBoxLayout>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include "ui/fields/GenderField.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "ui/UserUtils.h"

GenderField::GenderField(QWidget *parent)
  : QWidget(parent)
  , m_settings(ChatCore::i()->settings())
  , m_user(new User())
{
  m_combo = new QComboBox(this);
  m_combo->addItem(tr("Male"));
  m_combo->addItem(tr("Female"));
  m_combo->setMinimumWidth(m_combo->width());

  m_menu = new QMenu(this);

  addColor(tr("Default"));
  addColor(tr("Black"));
  addColor(tr("Gray"));
  addColor(tr("Green"));
  addColor(tr("Red"));
  addColor(tr("White"));
  addColor(tr("Yellow"));

  m_config = new QToolButton(this);
  m_config->setIcon(SCHAT_ICON(GearIcon));
  m_config->setMenu(m_menu);
  m_config->setPopupMode(QToolButton::InstantPopup);

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->addWidget(m_config);
  m_toolBar->setStyleSheet("QToolBar { margin:0px; border:0px; }" );

  m_user->gender().setRaw(m_settings->value(QLatin1String("Profile/Gender")).toInt());
  setState();

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_combo);
  mainLay->addWidget(m_toolBar);
  mainLay->addStretch();
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  connect(m_combo, SIGNAL(currentIndexChanged(int)), SLOT(indexChanged(int)));
  connect(m_settings, SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
}


void GenderField::updateData()
{
  m_settings->updateValue(QLatin1String("Profile/Gender"), m_user->gender().raw());
}


void GenderField::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void GenderField::indexChanged(int index)
{
  if (index == -1)
    return;

  if (index == 0)
    m_user->gender().set(Gender::Male);
  else
    m_user->gender().set(Gender::Female);

  setState();
  updateData();
}


void GenderField::setColor()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action)
    return;

  m_user->gender().setColor(action->data().toInt());

  setState();
  updateData();
}


void GenderField::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == QLatin1String("Profile/Gender")) {
    m_user->gender().setRaw(value.toInt());
    setState();
  }
}


void GenderField::addColor(const QString &name)
{
  QAction *action = m_menu->addAction(name, this, SLOT(setColor()));
  action->setCheckable(true);
  action->setData(m_colors.size());
  m_colors.append(action);
}


void GenderField::retranslateUi()
{
  m_combo->setItemText(0, tr("Male"));
  m_combo->setItemText(1, tr("Female"));

  m_colors.at(0)->setText(tr("Default"));
  m_colors.at(1)->setText(tr("Black"));
  m_colors.at(2)->setText(tr("Gray"));
  m_colors.at(3)->setText(tr("Green"));
  m_colors.at(4)->setText(tr("Red"));
  m_colors.at(5)->setText(tr("White"));
  m_colors.at(6)->setText(tr("Yellow"));
}


void GenderField::setIcons()
{
  int gender = m_user->gender().raw();


  m_user->gender().set(Gender::Male);
  m_combo->setItemIcon(0, UserUtils::icon(m_user));

  m_user->gender().set(Gender::Female);
  m_combo->setItemIcon(1, UserUtils::icon(m_user));

  m_user->gender().setRaw(gender);

  for (int i = 0; i < m_colors.size(); ++i) {
    m_user->gender().setColor(i);
    m_colors.at(i)->setIcon(UserUtils::icon(m_user));
  }

  m_user->gender().setRaw(gender);
}


void GenderField::setState()
{
  setIcons();
  m_config->setEnabled(true);

  if (m_user->gender().value() == Gender::Female)
    m_combo->setCurrentIndex(1);
  else if (m_user->gender().value() == Gender::Male)
    m_combo->setCurrentIndex(0);
  else {
    m_combo->setCurrentIndex(-1);
    m_config->setEnabled(false);
    return;
  }

  if (m_user->gender().color() > Gender::Yellow)
    return;

  for (int i = 0; i < m_colors.size(); ++i) {
    m_colors.at(i)->setChecked(false);
  }

  m_colors.at(m_user->gender().color())->setChecked(true);
}
