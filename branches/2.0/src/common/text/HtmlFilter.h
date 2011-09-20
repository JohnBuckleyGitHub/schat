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

#ifndef HTMLFILTER_H_
#define HTMLFILTER_H_

#include "schat.h"
#include "text/HtmlToken.h"

class QStringList;

/*!
 * Фильтрует и вырезает всё лишнее из HTML оставляя только минимальное
 * безопасное содержимое.
 */
class SCHAT_EXPORT HtmlFilter
{
public:
  HtmlFilter();
  int endTag(const QString &tag, QList<HtmlToken> &tokens, int pos = 0) const;
  QString filter(const QString &text) const;
  static void removeTags(QString &text, const QStringList &exclude);

private:
  void optimize(QList<HtmlToken> &tokens) const;
  void tokenize(const QString &text, QList<HtmlToken> &tokens) const;

  mutable bool m_optimize; ///< true если требуется оптимизация тегов.
};

#endif /* HTMLFILTER_H_ */
