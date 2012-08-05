/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 * Copyright © 2012 Alexey Ivanov <alexey.ivanes@gmail.com>
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

#include <QLabel>
#include <QVBoxLayout>
#include <QCheckBox>

#include "SpellCheckerWidget.h"
#include "sglobal.h"
#include "ChatCore.h"
#include "ChatSettings.h"

SpellCheckerWidget::SpellCheckerWidget(QWidget *parent)
  : QWidget(parent)
{
  m_label = new QLabel(this);
  m_active = new QCheckBox(this);
  m_active->setChecked(ChatCore::settings()->value(LS("SpellChecker/Active")).toBool());

  m_advanced = new QCheckBox(this);
  m_advanced->setChecked(ChatCore::settings()->value(LS("SpellChecker/Advanced")).toBool());

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(m_active);
  layout->addWidget(m_advanced);
  layout->setContentsMargins(10, 0, 3, 0);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_label);
  mainLay->addLayout(layout);
  mainLay->setContentsMargins(0, 12, 0, 0);

  retranslateUi();
}


void SpellCheckerWidget::retranslateUi()
{
  m_label->setText(LS("<b>") + tr("Spell checking") + LS("</b>"));
  m_active->setText(tr("Enable spell checking"));
  m_advanced->setText(tr("Advanced setup"));
}
