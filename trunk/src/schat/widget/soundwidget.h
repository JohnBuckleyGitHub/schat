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

#ifndef SOUNDWIDGET_H_
#define SOUNDWIDGET_H_

#include <QWidget>

class QCheckBox;
class QComboBox;
class QToolButton;
class Settings;

/*!
 * \brief Виджет обеспечивает выбор звука для определённого события.
 */
class SoundWidget : public QWidget
{
  Q_OBJECT

public:
  SoundWidget(const QString &key, const QString &name, const QString &desc, const QStringList &sounds, QWidget *parent = 0);
  int save();
  void reset(bool enable, const QString &file);

signals:
  void play(const QString &file);

private slots:
  void play();

private:
  QCheckBox *m_check;
  QComboBox *m_combo;
  QString m_key;
  QToolButton *m_play;
  Settings *m_settings;
};

#endif /* SOUNDWIDGET_H_ */
