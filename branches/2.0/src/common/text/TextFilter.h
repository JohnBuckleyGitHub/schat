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

#ifndef TEXTFILTER_H_
#define TEXTFILTER_H_

#include <QHash>
#include <QObject>
#include <QVariant>

#include "schat.h"


/*!
 * Базовый абстрактный класс фильтрации текста.
 */
class AbstractFilter
{
public:
  inline AbstractFilter()
  {}

  inline AbstractFilter(const QString &name)
  : m_name(name)
  {}

  inline QString name() const { return m_name; }
  virtual QString filter(const QString &text, QVariantHash options = QVariantHash()) const = 0;

protected:
  QString m_name;
};


class SCHAT_EXPORT TextFilter : public QObject
{
  Q_OBJECT

public:
  TextFilter(QObject *parent = 0);
  inline static TextFilter *i() { return m_self; }
  static bool add(AbstractFilter *filter);
  static QString filter(const QString &name, const QString &text, QVariantHash options = QVariantHash());
  static void removeAll();

private:
  static QHash<QString, AbstractFilter *> m_filters; ///< Доступные фильтры текса.
  static TextFilter *m_self; ///< Указатель на себя.
};


#endif /* TEXTFILTER_H_ */
