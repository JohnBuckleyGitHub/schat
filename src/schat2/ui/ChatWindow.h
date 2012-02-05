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

#ifndef CHATWINDOW_H_
#define CHATWINDOW_H_

#include <QMainWindow>
#include <QTime>

class ChatSettings;
class Notify;
class QVBoxLayout;
class SendWidget;
class StatusBar;
class TabWidget;

class ChatWindow : public QMainWindow
{
  Q_OBJECT

public:
  ChatWindow(QWidget *parent = 0);
  void showChat();

protected:
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void resizeEvent(QResizeEvent *event);
  #if defined(Q_WS_WIN)
  bool winEvent(MSG *message, long *result);
  #endif

private slots:
  void closeChat();
  void notify(const Notify &notify);
  void pageChanged(int type, bool visible);
  void settingsChanged(const QString &key, const QVariant &value);

private:
  void hideChat();
  void retranslateUi();

  #if defined(Q_WS_WIN)
  void setWindowsAero();
  #endif

  ChatSettings *m_settings;  ///< Настройки.
  QTime m_activationChanged; ///< Время когда была изменена видимость окна.
  QVBoxLayout *m_mainLay;    ///< Основной компоновщик.
  QWidget *m_central;        ///< Центральный виджет.
  SendWidget *m_send;        ///< Виджет отправки сообщения.
  StatusBar *m_statusBar;    ///< Статус бар.
  TabWidget *m_tabs;         ///< Вкладки.
};

#endif /* CHATWINDOW_H_ */
