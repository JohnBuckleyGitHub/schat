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

#ifndef ABSTRACTTAB_H_
#define ABSTRACTTAB_H_

#include <QIcon>
#include <QWidget>

class TabWidget;

class AbstractTab : public QWidget
{
  Q_OBJECT

public:
  /// Тип виджета.
  enum TabType {
    UnknownType,
    ChatViewType,
    ChannelType,
    PrivateType
  };

  AbstractTab(const QByteArray &id, TabType type, TabWidget *parent);
  inline QAction *action() const { return m_action; }
  inline QByteArray id() const { return m_id; }
  inline TabType type() const { return m_type; }
  virtual void setOnline(bool online = true);

signals:
  void actionTriggered(bool checked = false);

protected:
  virtual void retranslateUi();
  void changeEvent(QEvent *event);

  QAction *m_action;
  QIcon m_icon;
  TabWidget *m_tabs;

private:
  QByteArray m_id;
  TabType m_type;
};

#endif /* ABSTRACTTAB_H_ */
