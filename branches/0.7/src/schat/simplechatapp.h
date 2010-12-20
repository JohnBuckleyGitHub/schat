/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#ifndef SIMPLECHATAPP_H_
#define SIMPLECHATAPP_H_

#ifdef SCHAT_NO_SINGLEAPP
 #define QtSingleApplication QApplication
 #include <QApplication>
#else
 #include <QtSingleApplication>
#endif

class SChatWindow;
class Translation;

/*!
 * \brief Экземпляр приложения.
 */
class SimpleChatApp : public QtSingleApplication
{
  Q_OBJECT

public:
  SimpleChatApp(int &argc, char **argv);
  ~SimpleChatApp();
  bool isRunning();
  int run();
  static QIcon iconFromTheme(const QString &name);
  static SimpleChatApp *instance();
  Translation *translation();

  #if !defined(SCHAT_NO_STYLE)
  static QString defaultStyle();
  #endif

private:
  SChatWindow *m_window;
  Translation *m_translation;
};

#endif /* SIMPLECHATAPP_H_ */
