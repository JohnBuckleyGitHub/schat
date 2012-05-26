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

#ifndef TOOLBARACTION_H_
#define TOOLBARACTION_H_

#include <QSharedPointer>

#include "schat.h"

class QAction;

/*!
 * Базовый класс для действий, размещённых на панели инструментов.
 */
class SCHAT_CORE_EXPORT ToolBarActionCreator
{
public:
  /// Тип действия.
  enum Type {
    Action, ///< QAction.
    Widget  ///< QWidget.
  };

  ToolBarActionCreator(int weight, const QString &name = QString());
  virtual ~ToolBarActionCreator() {}
  inline bool isActionType() const       { return m_type == Action; }
  inline QAction* action() const         { return m_action; }
  inline const QString& name() const     { return m_name; }
  inline int weight() const              { return m_weight; }
  inline Type type() const               { return m_type; }
  inline void setAction(QAction *action) { m_action = action; }
  virtual QAction* createAction(QObject *parent = 0) const;

protected:
  QString m_name;    ///< Машинное имя действия.
  Type m_type;       ///< Тип действия \sa Type.

private:
  int m_weight;      ///< Вес, используется для задания порядка.
  QAction *m_action; ///< Действие, равно 0 если действие не размещено на тулбаре.
};

typedef QSharedPointer<ToolBarActionCreator> ToolBarAction;

#endif /* TOOLBARACTION_H_ */
