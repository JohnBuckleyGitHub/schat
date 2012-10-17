/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CONSOLEVIEW_H_
#define CONSOLEVIEW_H_

#include <QWebView>
#include <QVariant>

class ConsoleTr;
class Notify;

class ConsoleView : public QWebView
{
  Q_OBJECT

public:
  ConsoleView(QWidget *parent = 0);
  ~ConsoleView();

signals:
  void feed(const QVariantMap &data);

private slots:
  void notify(const Notify &notify);
  void populateJavaScriptWindowObject();

private:
  ConsoleTr *m_tr; ///< Перевод.
};

#endif /* CONSOLEVIEW_H_ */
