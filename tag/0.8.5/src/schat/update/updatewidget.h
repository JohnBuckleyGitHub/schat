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
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UPDATEWIDGET_H_
#define UPDATEWIDGET_H_

#include <QWidget>

class QLabel;
class QMovie;
class Settings;

/*!
 * \brief Виджет проверяющий состояние обновления.
 */
class UpdateWidget : public QWidget
{
  Q_OBJECT

public:
  UpdateWidget(QWidget *parent = 0);

public slots:
  void start();

private slots:
  void linkActivated(const QString &link);
  void notify(int code);

private:
  void setIcon(const QString &icon);

  QLabel *m_icon;
  QLabel *m_movie;
  QLabel *m_text;
  Settings *m_settings;
};

#endif /* UPDATEWIDGET_H_ */
