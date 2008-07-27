/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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

#ifndef PROFILEWIDGET_H_
#define PROFILEWIDGET_H_

#include <QWidget>

class AbstractProfile;
class QComboBox;
class QLabel;
class QLineEdit;

class ProfileWidget : public QWidget {
  Q_OBJECT

public:
  ProfileWidget(AbstractProfile *p, QWidget *parent = 0);
  inline bool isModifiled() { return m_modifiled; }
  void reset();
  void save();
  
signals:
  void validNick(bool b);
  
private slots:
  void validateNick(const QString &text);
  
private:
  AbstractProfile *m_profile;
  bool m_modifiled;
  QComboBox *m_gender;
  QLabel *m_genderLabel;
  QLabel *m_nameLabel;
  QLabel *m_nickLabel;
  QLineEdit *m_name;
  QLineEdit *m_nick;
};

#endif /*PROFILEWIDGET_H_*/
