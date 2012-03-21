/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#ifndef PROFILEBUTTON_H_
#define PROFILEBUTTON_H_

#include <QToolButton>

/*!
 * Кнопка добавления полей профиля.
 */
class ProfileButton : public QToolButton
{
  Q_OBJECT

public:
  ProfileButton(QWidget *parent = 0);

signals:
  void add(const QString &field);

private slots:
  void rebuild();
  void triggered(QAction *action);

private:
  QMenu *m_menu; ///< Меню.
};

#endif /* PROFILEBUTTON_H_ */
