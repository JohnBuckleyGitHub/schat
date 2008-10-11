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

#ifndef WELCOMEDIALOG_H_
#define WELCOMEDIALOG_H_

#include <QDialog>
#include <QPushButton>

class NetworkWidget;
class AbstractProfile;
class ProfileWidget;
class QCheckBox;
class QLabel;
class QLineEdit;
class SChatWindow;
class Settings;

/*!
 * \brief Диалог приветствия.
 */
class WelcomeDialog : public QDialog
{
  Q_OBJECT

public:
  WelcomeDialog(AbstractProfile *profile, QWidget *parent = 0);

public slots:
  void accept();

private slots:
  inline void validNick(bool b) { m_okButton->setEnabled(b); }
  void changeIcon(bool s);

private:
  NetworkWidget *m_networkWidget;
  ProfileWidget *m_profileWidget;
  QCheckBox *m_askCheckBox;
  QPushButton *m_moreButton;
  QPushButton *m_okButton;
  Settings *m_settings;
};

#endif /*WELCOMEDIALOG_H_*/
