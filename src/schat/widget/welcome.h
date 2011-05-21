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

#ifndef WELCOME_H_
#define WELCOME_H_

#include "translatewidget.h"

class LanguageBox;
class NetworkWidget;
class ProfileWidget;
class QCheckBox;
class QGridLayout;
class QPushButton;

class WelcomeWidget : public TranslateWidget
{
  Q_OBJECT

public:
  WelcomeWidget(QWidget *parent = 0);
  void notify(int code);

protected:
  void keyPressEvent(QKeyEvent *event);

private slots:
  void languageChanged(const QString &text);
  void link();
  void validNick(bool valid);

private:
  void retranslateUi();

  LanguageBox *m_language;
  NetworkWidget *m_network;
  ProfileWidget *m_profile;
  QCheckBox *m_ask;
  QGridLayout *m_grid;
  QPushButton *m_connect;
};

#endif /* WELCOME_H_ */
