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

#ifndef MAINCHANNEL_H_
#define MAINCHANNEL_H_

#include <QWidget>

#include "abstracttab.h"

class NetworkWidget;
class QSplitter;
class QTabWidget;
class QVBoxLayout;
class UserView;
class WelcomeWidget;

/*!
 * \brief Обеспечивает поддержку главного канала чата.
 */
class MainChannel : public AbstractTab
{
  Q_OBJECT

public:
  MainChannel(const QIcon &icon, UserView *userView, QTabWidget *parent);
  void addNewUser(quint8 gender, const QString &nick);
  void addUserLeft(quint8 gender, const QString &nick, const QString &bye);
  void displayChoiceServer(bool display);
  void displayWelcome(bool display);

protected:
  void changeEvent(QEvent *event);

private slots:
  void notify(int code);
  void splitterMoved();
  void usersCountChanged(int count);

private:
  QWidget* createUserView();
  void retranslateUi();

  int m_count;
  NetworkWidget *m_networkWidget;
  QSplitter *m_splitter;
  QTabWidget *m_tabs;
  QVBoxLayout *m_mainLayout;
  UserView *m_userView;
  WelcomeWidget *m_welcome;
};

#endif /*MAINCHANNEL_H_*/
