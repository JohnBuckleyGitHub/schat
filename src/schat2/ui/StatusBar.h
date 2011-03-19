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

class QLabel;
class QProgressIndicator;
class SimpleClient;

class StatusBar : public QStatusBar
{
  Q_OBJECT

public:
  StatusBar(SimpleClient *client, QWidget *parent = 0);

public slots:
  inline void setUrl(const QString &url) { m_url = url; }

protected:
  bool event(QEvent *event);
  void changeEvent(QEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

private slots:
  void clientStateChanged(int state);

private:
  void retranslateUi();
  void updateStyleSheet();

  int m_clientState;              ///< Состояние клиента.
  QLabel *m_icon;                 ///< Главная иконка.
  QLabel *m_label;                ///< Информация о состоянии.
  QLabel *m_secure;               ///< Иконка безопасного соединения.
  QProgressIndicator *m_progress; ///< Отображает состояние подключения.
  QUrl m_url;
  SimpleClient *m_client;         ///< Клиент.
};

#endif /* STATUSBAR_H_ */
