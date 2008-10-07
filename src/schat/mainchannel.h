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

#ifndef MAINCHANNEL_H_
#define MAINCHANNEL_H_

#include <QLineEdit>
#include <QWidget>

#include "abstracttab.h"

class NetworkWidget;
class QAction;
class QHBoxLayout;
class QLabel;
class QToolButton;
class QVBoxLayout;
class Settings;

class MainChannel : public AbstractTab {
  Q_OBJECT

public:
  MainChannel(Settings *settings, QWidget *parent = 0);
  void displayChoiceServer(bool display);

private slots:
  void serverChanged();

private:
  void createActions();

  NetworkWidget *m_networkWidget;
  QAction *m_connectCreateAction;
  QHBoxLayout *m_networkLayout;
  QToolButton *m_connectCreateButton;
  QVBoxLayout *m_mainLayout;
  Settings *m_settings;
};

#endif /*MAINCHANNEL_H_*/
