/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#include "progresspage.h"
#include "wizardsettings.h"

/*!
 * \brief Конструктор класса ProgressPage.
 */
ProgressPage::ProgressPage(QWidget *parent)
  : QWizardPage(parent)
{
  m_settings = settings;
  setTitle(tr("Идёт создание дистрибутива"));
  setSubTitle(tr("Подождите идёт создание дистрибутива"));

  m_label = new QLabel(this);
  m_progress = new QProgressBar(this);
  m_log = new QTextEdit(this);
  m_log->setReadOnly(true);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_label);
  mainLay->addWidget(m_progress);
  mainLay->addWidget(m_log);
  mainLay->setMargin(0);
}
