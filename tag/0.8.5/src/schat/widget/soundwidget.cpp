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

#include <QtGui>

#include "soundwidget.h"
#include "settings.h"

/*!
 * \brief Конструктор класса SoundWidget.
 */
SoundWidget::SoundWidget(const QString &key, const QString &name, const QString &desc, const QStringList &sounds, QWidget *parent)
  : QWidget(parent), m_key(key)
{
  m_settings = SimpleSettings;

  m_check = new QCheckBox(name, this);
  m_check->setToolTip(desc);
  m_check->setChecked(m_settings->getBool("Sound/" + key + "Enable"));

  m_combo = new QComboBox(this);
  m_combo->addItems(sounds);

  m_play = new QToolButton(this);
  m_play->setIcon(QIcon(":/images/player_play.png"));
  m_play->setAutoRaise(true);

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_check);
  mainLay->addStretch();
  mainLay->addWidget(m_combo);
  mainLay->addWidget(m_play);
  mainLay->setMargin(0);
  mainLay->setSpacing(1);

  connect(m_check, SIGNAL(clicked(bool)), m_combo, SLOT(setEnabled(bool)));
  connect(m_check, SIGNAL(clicked(bool)), m_play,  SLOT(setEnabled(bool)));
  connect(m_play,  SIGNAL(clicked(bool)), SLOT(play()));

  m_combo->setEnabled(m_check->isChecked());
  m_play->setEnabled(m_check->isChecked());

  m_combo->setCurrentIndex(m_combo->findText(m_settings->getString("Sound/" + key)));
}


int SoundWidget::save()
{
  int modified = 0;
  modified += m_settings->save("Sound/" + m_key + "Enable", m_check->isChecked());
  modified += m_settings->save("Sound/" + m_key, m_combo->currentText());

  return modified;
}


void SoundWidget::reset(bool enable, const QString &file)
{
  m_check->setChecked(enable);
  m_combo->setCurrentIndex(m_combo->findText(file));
  m_combo->setEnabled(m_check->isChecked());
  m_play->setEnabled(m_check->isChecked());
}


void SoundWidget::play()
{
  QString file = "/" + m_combo->currentText();
  foreach (QString path, SimpleSettings->path(Settings::SoundsPath)) {
    if (QFile::exists(path + file)) {
      emit play(path + file);
      return;
    }
  }
}
