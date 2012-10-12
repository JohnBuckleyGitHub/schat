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

#ifndef ABSTRACTTAB_H_
#define ABSTRACTTAB_H_

#include <QIcon>
#include <QWidget>

#include "schat.h"

class QMenu;
class TabWidget;

class SCHAT_CORE_EXPORT AbstractTab : public QWidget
{
  Q_OBJECT

public:
  /// Опции вкладки.
  enum Option {
    NoOptions      = 0, ///< Нет опций.
    CanSendMessage = 1, ///< Вкладка поддерживает отправку сообщений.
  };

  Q_DECLARE_FLAGS(Options, Option)

  AbstractTab(const QByteArray &id, const QString &type, TabWidget *parent);
  inline bool isDeleteOnClose() const { return m_deleteOnClose; }
  inline bool isOnline() const        { return m_online; }
  inline const QByteArray& id() const { return m_id; }
  inline const QIcon& icon() const    { return m_icon; }
  inline const QString& text() const  { return m_text; }
  inline const QString& type() const  { return m_type; }
  inline Options options() const      { return m_options; }
  inline QAction *action() const      { return m_action; }
  virtual bool bindMenu(QMenu *menu)  { Q_UNUSED(menu) return false; }
  virtual void setOnline(bool online = true);
  void setIcon(const QIcon &icon);
  void setText(const QString &text);

signals:
  void actionTriggered(bool checked = false);

protected:
  inline void setId(const QByteArray &id) { m_id = id; }
  virtual void retranslateUi();
  void changeEvent(QEvent *event);

  bool m_deleteOnClose; ///< true если вкладку нужно удалить после закрытия.
  bool m_online;        ///< true если вкладка "В сети".
  Options m_options;    ///< Опции вкладки.
  QAction *m_action;    ///< Действие используемое для открытие вкладки.
  QIcon m_icon;         ///< Нормальная иконка вкладки.
  QString m_text;       ///< Заголовок вкладки.
  TabWidget *m_tabs;    ///< Указатель на виджет вкладок.

private:
  QByteArray m_id;
  QString m_type;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractTab::Options)

#endif /* ABSTRACTTAB_H_ */