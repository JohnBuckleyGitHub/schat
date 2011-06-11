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

#ifndef TABWIDGET_H_
#define TABWIDGET_H_

#include <QHash>
#include <QTabWidget>
#include <QPointer>

class AboutTab;
class AbstractMessage;
class AbstractTab;
class AlertTab;
class Channel;
class ChannelTab;
class ClientMessage;
class MessageData;
class PrivateTab;
class QMenu;
class QToolBar;
class QToolButton;
class SettingsTab;
class SimpleClient;
class SoundButton;
class TabBar;
class WelcomeTab;
/*!
 * Класс, обеспечивающий отображение и управление вкладками.
 */
class TabWidget : public QTabWidget
{
  Q_OBJECT

public:
  TabWidget(QWidget *parent = 0);
  AbstractTab *widget(int index) const;
  inline TabBar *tabBar() { return m_tabBar; }
  QByteArray currentId() const;

signals:
  void pageChanged(int type, bool visible);

protected:
  void changeEvent(QEvent *event);

private slots:
  void about();
  void addPrivateTab(const QByteArray &id);
  void closeTab(int index);
  void currentChanged(int index);
  void hideMainMenu();
  void notify(int notice, const QVariant &data);
  void openTab();
  void quit();
  void settings();
  void showMainMenu();

  void clientStateChanged(int state);
  void join(const QByteArray &channelId, const QByteArray &userId, int option = 0);
  void join(const QByteArray &channelId, const QList<QByteArray> &usersId);
  void message(const AbstractMessage &data);
  void message(int status, const MessageData &data);
  void part(const QByteArray &channelId, const QByteArray &userId);
  void updateUserData(const QByteArray &userId);
  void userLeave(const QByteArray &userId);

private:
  ChannelTab *createChannelTab(const QByteArray &id);
  PrivateTab *privateTab(const QByteArray &id, bool create = true, bool show = false);
  void aboutTab();
  void chatTab(AbstractTab *tab, bool show = true);
  void createToolBars();
  void displayChannelUserCount(const QByteArray &id);
  void retranslateUi();
  void settingsTab();
  void showWelcome();

  AlertTab *m_alertTab;                      ///< Вкладка оповещений.
  QAction *m_aboutAction;                    ///< О Simple Chat.
  QAction *m_quitAction;                     ///< Quit.
  QAction *m_settingsAction;                 ///< Settings.
  QHash<QByteArray, ChannelTab*> m_channels; ///< Таблица каналов.
  QHash<QByteArray, PrivateTab*> m_talks;    ///< Таблица приватных разговоров.
  QMenu *m_channelsMenu;                     ///< Меню каналов.
  QMenu *m_mainMenu;                         ///< Главное меню.
  QMenu *m_settingsMenu;                     ///< Меню для кнопки m_settingsButton.
  QMenu *m_talksMenu;                        ///< Меню разговоров.
  QPointer<AboutTab> m_aboutTab;             ///< О Simple Chat.
  QPointer<SettingsTab> m_settingsTab;       ///< Настройка.
  QToolBar *m_leftToolBar;                   ///< Левая панель инструментов.
  QToolBar *m_rightToolBar;                  ///< Правая панель инструментов.
  QToolButton *m_menuButton;                 ///< Кнопка с меню пользователей и каналов.
  QToolButton *m_settingsButton;             ///< Кнопка с меню настроек.
  SimpleClient *m_client;                    ///< Клиент чата.
  SoundButton *m_soundButton;                ///< Кнопка включения/выключения звука.
  TabBar *m_tabBar;                          ///< Заголовок виджета.
  WelcomeTab *m_welcomeTab;                  ///< Вкладка приветствия.
};

#endif /* TABWIDGET_H_ */
