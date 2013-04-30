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

#ifndef TABWIDGET_H_
#define TABWIDGET_H_

#include <QMap>
#include <QTabWidget>

#include "Channel.h"
#include "schat.h"

class AboutTab;
class AbstractMessage;
class AbstractTab;
class AuthIcon;
class ChannelBaseTab;
class ChannelTab;
class MainToolBar;
class Message;
class Notify;
class PrivateTab;
class ProgressTab;
class ServerTab;
class SettingsTab;
class TabBar;
class TabsToolBar;
class TrayIcon;
class WelcomeTab;

/*!
 * Класс, обеспечивающий отображение и управление вкладками.
 */
class SCHAT_CORE_EXPORT TabWidget : public QTabWidget
{
  Q_OBJECT

public:
  TabWidget(QWidget *parent = 0);
  ~TabWidget();

  AbstractTab *widget(int index) const;
  ClientChannel channel(const QByteArray &id) const;
  inline ServerTab *serverTab() const            { return m_serverTab; }
  inline TabBar *tabBar() const                  { return m_tabBar; }
  inline TabsToolBar *toolBar() const            { return m_tabsToolBar; }
  static TabWidget *i()                          { return m_self; }

  int showPage(AbstractTab *tab, bool current = true);
  int showPage(const QByteArray &id);
  static AbstractTab *page(const QByteArray &id) { return m_self->m_pages.value(id); }
  void closePage(const QByteArray &id);

  ChannelBaseTab *channelTab(const QByteArray &id, bool create = true, bool show = true);
  static bool isActive(const QByteArray &id);
  static bool isActiveChatWindow();
  static bool isCurrent(AbstractTab *tab);
  static void add(const Message &message, bool create = true);
  void stopAlert();

signals:
  void pageChanged(AbstractTab *tab);
  void pinned(AbstractTab *tab);
  void unpinned(AbstractTab *tab);

public slots:
  void closeTab(int index);

protected:
  bool event(QEvent *event);
  void changeEvent(QEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void tabInserted(int index);
  void tabRemoved(int index);

private slots:
  void currentChanged(int index);
  void notify(const Notify &notify);
  void openTab();

  void addChannel(const QByteArray &id);

  void clientStateChanged(int state, int previousState);

private:
  int addChatTab(AbstractTab *tab);
  int tabAt(const QPoint &pos) const;
  void addImpl(const Message &message, bool create = true);
  void createToolBars();
  void lastTab();
  void retranslateUi();
  void showWelcome();
  void unpin(AbstractTab *tab);

  AuthIcon *m_authIcon;                         ///< Иконка провайдеров авторизации.
  MainToolBar *m_mainToolBar;                   ///< Правая панель инструментов.
  QList<QByteArray> m_prefetch;                 ///< Список каналов для которых было запрошено создание вкладки, но они не были созданы.
  QMap<QByteArray, AbstractTab*> m_pages;       ///< Вкладки не связанные с каналами.
  QMap<QByteArray, ChannelBaseTab*> m_channels; ///< Таблица каналов.
  ServerTab *m_serverTab;                       ///< Вкладка сервера.
  static TabWidget *m_self;                     ///< Указатель на себя.
  TabBar *m_tabBar;                             ///< Заголовок виджета.
  TabsToolBar *m_tabsToolBar;                   ///< Левая панель инструментов.
  TrayIcon *m_tray;                             ///< Иконка в трее.
};

#endif /* TABWIDGET_H_ */