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

#ifndef NICKEDIT_H_
#define NICKEDIT_H_

#include <QWidget>

class QLineEdit;

/*!
 * \brief Обеспечивает редактирование ника и пола пользователя.
 */
class NickEdit : public QWidget
{
  Q_OBJECT

public:
  NickEdit(const QString &nick, QWidget *parent = 0);
  QString nick() const;
  void reset();

signals:
  void validNick(bool valid);

private slots:
  void validateNick(const QString &text);

private:
  QLineEdit *m_edit;
};

#endif /* NICKEDIT_H_ */
