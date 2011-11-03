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

#ifndef NETWORKWIDGET_H_
#define NETWORKWIDGET_H_

#include <QWidget>

class NetworkManager;
class QComboBox;
class QMenu;
class QToolBar;
class QToolButton;
class SimpleClient;

class NetworkWidget : public QWidget
{
  Q_OBJECT

public:
  NetworkWidget(QWidget *parent = 0);
  QAction *connectAction();

public slots:
  void open();

protected:
  void changeEvent(QEvent *event);
  void keyPressEvent(QKeyEvent *event);

private slots:
  void add();
  void edit();
  void indexChanged(int index);
  void notify(int notice, const QVariant &data);
  void remove();
  void showMenu();

private:
  int isCurrentActive() const;
  void load();
  void retranslateUi();
  void updateIndex();
  void updateIndex(const QByteArray &id);
  void updateIndex(const QString &url);

  NetworkManager *m_manager; ///< Указатель на менеджер сетевых подключений.
  QAction *m_addAction;
  QAction *m_connectAction;
  QAction *m_edit;
  QAction *m_removeAction;
  QComboBox *m_combo;
  QMenu *m_menu;
  QToolBar *m_toolBar;
  QToolButton *m_config;
  SimpleClient *m_client;   ///< Указатель на клиент.
};

#endif /* NETWORKWIDGET_H_ */
