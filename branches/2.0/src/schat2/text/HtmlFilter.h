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

#include "text/TextFilter.h"
#include "schat.h"

class QStringList;

class HtmlToken
{
public:
  enum Type {
    Undefined,
    StartTag,
    EndTag,
    Text,
    Tag
  };

  inline HtmlToken()
  : simple(true)
  , type(Undefined)
  {}

  inline HtmlToken(const QString &text)
  : simple(true)
  , text(text)
  , type(Text)
  {}

  inline HtmlToken(Type type, const QString &text)
  : simple(true)
  , text(text)
  , type(type)
  {
    if (type != Tag)
      return;

    tag = text.mid(1, text.size() - 2);
    if (tag.startsWith(QLatin1Char('/'))) {
      this->type = EndTag;
      tag.remove(0, 1);
    }
    else {
      this->type = StartTag;
      int space = tag.indexOf(QLatin1Char(' '));
      if (space != -1) {
        simple = false;
        tag.remove(space, tag.size() - space);
      }
    }

    if (tag.isEmpty())
      this->type = Undefined;

    tag = tag.toLower();
  }

  QString toEndTag() const
  {
    if (type != StartTag)
      return QString();

    return QLatin1String("</") + tag + QLatin1Char('>');
  }

  bool simple;  ///< false в случае если начальный тег содержит дополнительные данные.
  QString tag;  ///< Тег, в нижнем регистре и без обрамления.
  QString text; ///< Текстовое содержимое.
  Type type;    ///< Тип.
};

/*!
 * Фильтрует и вырезает всё лишнее из HTML оставляя только минимальное
 * безопасное содержимое.
 */
class SCHAT_CORE_EXPORT HtmlFilter : public AbstractFilter
{
public:
  HtmlFilter();
  QString filter(const QString &text, QVariantHash options = QVariantHash()) const;
  static void removeTags(QString &text, const QStringList &exclude);

private:
  void optimize(QList<HtmlToken> &tokens) const;
  void tokenize(const QString &text, QList<HtmlToken> &tokens) const;

  mutable bool m_optimize; ///< true если требуется оптимизация тегов.
};

#endif /* HTMLFILTER_H_ */
