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

#include <QTextDocument>
#include <QUrl>

#include "net/SimpleID.h"
#include "text/UrlFilter.h"
#include "ChatUrls.h"

UrlFilter::UrlFilter()
  : AbstractFilter("Url")
{
}


bool UrlFilter::filter(QList<HtmlToken> &tokens, QVariantHash options) const
{
  Q_UNUSED(options)

  QString name;

  for (int i = 0; i < tokens.size(); ++i) {

    if (tokens.at(i).type == HtmlToken::StartTag && tokens.at(i).tag == "a") {
      HtmlATag tag(tokens.at(i));

      if (tag.url.startsWith("chat://channel/")) {
        tag.classes = "nick";
        ClientChannel user = ChatUrls::channel(QUrl(tag.url));
        if (user) {
          tag.classes += " " + SimpleID::encode(user->id());
          name = user->name();
        }

        tokens[i].text = tag.toText();
      }
    }
    else if (tokens.at(i).type == HtmlToken::Text && !name.isEmpty()) {
      tokens[i].text = Qt::escape(name);
      name.clear();
    }
  }

  return true;
}
