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

#ifndef STATUSBAR_H_
#define STATUSBAR_H_

#include <QObject>
#include <QStatusBar>
#include <QUrl>

#include "schat.h"

class NetworkWidget;
class QLabel;
class QProgressIndicator;
class QWidgetAction;
class StatusMenu;
class StatusWidget;

class SCHAT_CORE_EXPORT StatusBar : public QStatusBar
{
  Q_OBJECT

public:
  StatusBar(QWidget *parent = 0);

protected:
  bool event(QEvent *event);
  void changeEvent(QEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

private slots:
  void clientStateChanged(int state);
  void loggedIn(const QString &name);
  void menuTriggered(QAction *action);

private:
  void menu(const QPoint &point);
  void retranslateUi();
  void updateStyleSheet();

  NetworkWidget *m_url;           ///< Поле ввода адреса сервера.
  QAction *m_connect;             ///< Главное действие связанное с сетевым подключением.
  QLabel *m_icon;                 ///< Главная иконка.
  QLabel *m_label;                ///< Информация о состоянии.
  QLabel *m_login;
  QLabel *m_secure;               ///< Иконка безопасного соединения.
  QProgressIndicator *m_progress; ///< Отображает состояние подключения.
  QWidgetAction *m_urlAction;     ///< Действие для добавления в меню поля ввода адреса сервера.
  StatusWidget *m_status;         ///< Виджет выбора статуса.
};

#endif /* STATUSBAR_H_ */
