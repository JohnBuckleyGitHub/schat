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

#ifndef PROFILEWIDGET_H_
#define PROFILEWIDGET_H_

#include <QWidget>

class AbstractProfile;
class QComboBox;
class QLineEdit;
class NickEdit;

/*!
 * \brief Виджет обеспечивающий редактирование собственного профиля.
 */
class ProfileWidget : public QWidget {
  Q_OBJECT

public:
  ProfileWidget(bool compactGender, QWidget *parent = 0);
  int save();
  void reset();

signals:
  void validNick(bool valid);

private:
  AbstractProfile *m_profile;
  bool m_compactGenderWidget;
  bool m_maxRecentItems;
  NickEdit *m_nickEdit;
  QComboBox *m_gender;
  QLineEdit *m_name;
};

#endif /*PROFILEWIDGET_H_*/
