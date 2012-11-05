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

#ifndef SOUNDACTION_H_
#define SOUNDACTION_H_

#include <QObject>
#include <QAction>

/*!
 * \brief Обвёртка для самодостаточного управления состоянием действия включения/выключения звука.
 */
class SoundAction : public QAction
{
  Q_OBJECT

public:
  SoundAction(QObject *parent = 0);
  inline bool isMute() const { return m_mute; };
  void mute();
  void mute(bool mute);
  void retranslateUi();

private slots:
  void notify(int notify);
  void sound();

private:
  bool m_mute; ///< \a true если звук отключен.
};

#endif /* SOUNDACTION_H_ */
