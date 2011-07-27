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

#ifndef GENDERFIELD_H_
#define GENDERFIELD_H_

#include <QWidget>

#include "User.h"

class ChatSettings;
class QComboBox;
class QMenu;
class QToolBar;
class QToolButton;

class GenderField : public QWidget
{
  Q_OBJECT

public:
  GenderField(QWidget *parent = 0);

protected:
  virtual void updateData();
  void changeEvent(QEvent *event);

private slots:
  void indexChanged(int index);
  void setColor();
  void settingsChanged(const QString &key, const QVariant &value);

private:
  void addColor(const QString &name);
  void retranslateUi();
  void setIcons();
  void setState();

  ChatSettings *m_settings;
  ClientUser m_user;
  QComboBox *m_combo;
  QList<QAction *> m_colors;
  QMenu *m_menu;
  QToolBar *m_toolBar;
  QToolButton *m_config;
};

#endif /* GENDERFIELD_H_ */
