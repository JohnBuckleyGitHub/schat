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

#ifndef ABSTRACTTAB_H_
#define ABSTRACTTAB_H_

#include <QIcon>
#include <QWidget>

#include "chatbrowser.h"

class Settings;
class QTextBrowser;

/*!
 * \brief Абстрактный базовый класс для вкладок чата.
 */
class AbstractTab : public QWidget {
  Q_OBJECT

public:
  enum Type {
    Unknown,
    Main,
    Private,
    Direct,
    DirectServer
  };

  AbstractTab(Settings *settings, QWidget *parent = 0);

  bool notice;
  ChatBrowser *browser;
  QIcon icon;
  Type type;

signals:
  void nickClicked(const QString &hex);
};

#endif /*ABSTRACTTAB_H_*/
