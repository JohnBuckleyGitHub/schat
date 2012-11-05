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

#include "abstractprofile.h"
#include "protocol.h"
#include "settings.h"
#include "soundaction.h"

/*!
 * Конструктор класса SoundAction.
 */
SoundAction::SoundAction(QObject *parent)
  : QAction(parent)
{
  setData("sound");
  mute();
  connect(this, SIGNAL(triggered()), SLOT(sound()));
  connect(SimpleSettings, SIGNAL(changed(int)), SLOT(notify(int)));
}


/*!
 * Включает/выключает звук.
 */
void SoundAction::mute()
{
  mute(!SimpleSettings->getBool("Sound"));
}


/*!
 * Включает/выключает звук.
 *
 * \param mute \a true выключить звук.
 */
void SoundAction::mute(bool mute)
{
  if (mute)
    setIcon(QIcon(":/images/sound_mute.png"));
  else
    setIcon(QIcon(":/images/sound.png"));

  m_mute = mute;
  retranslateUi();
}


void SoundAction::retranslateUi()
{
  if (m_mute)
    setText(tr("Turn on sounds"));
  else
    setText(tr("Turn off sounds"));
}


/*!
 * Обработка изменения настроек.
 */
void SoundAction::notify(int notify)
{
  if (notify == Settings::SoundChanged) {
    mute();
    if (SimpleSettings->profile()->status() == schat::StatusDnD && SimpleSettings->getBool("Sound/MuteInDnD"))
      mute(true);
  }
}


/*!
 * Включение/выключение звука пользователем.
 * Если текущий статус равен schat::StatusDnD и звук включен,
 * то опция \b Sound/MuteInDnD устанавливается в \a true и звук выключается.
 * иначе если звук выключен, звук включается глобально, опция \b Sound и
 * опция \b Sound/MuteInDnD устанавливается в \a false.
 *
 * Для всех прочих статусов, значение опции \b Sound меняется на противоположное.
 */
void SoundAction::sound()
{
  if (SimpleSettings->profile()->status() == schat::StatusDnD) {
    if (!isMute()) {
      SimpleSettings->setBool("Sound/MuteInDnD", true);
      mute(true);
    }
    else {
      SimpleSettings->setBool("Sound/MuteInDnD", false);
      SimpleSettings->setBool("Sound", true);
      mute(false);
    }
  }
  else {
    SimpleSettings->setBool("Sound", !SimpleSettings->getBool("Sound"));
    mute();
  }
}
