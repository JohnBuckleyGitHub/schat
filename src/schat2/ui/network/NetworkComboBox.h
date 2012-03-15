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

#ifndef NETWORKCOMBOBOX_H_
#define NETWORKCOMBOBOX_H_

#include <QComboBox>

class Notify;

class NetworkComboBox : public QComboBox
{
  Q_OBJECT

public:
  NetworkComboBox(QWidget *parent = 0);
  void load();

public slots:
  void remove();

private slots:
  void add(const QString &url = QLatin1String("schat://"));
  void edit();
  void indexChanged(int index);
  void notify(const Notify &notify);

private:
  void updateIndex();

  QByteArray m_editing; ///< Идентификатор редактируемой сети.
  QByteArray m_tmpId;   ///< Временный идентификатор для текущей редактируемой сети.
};

#endif /* NETWORKCOMBOBOX_H_ */
