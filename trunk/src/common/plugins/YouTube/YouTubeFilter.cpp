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

#include <QUrl>
#include <QUuid>

#include "YouTubeFilter.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include <QDebug>

YouTubeFilter::YouTubeFilter()
  : AbstractFilter(950)
{
    m_urls << "http://www.youtube.com/watch?v=" << "http://youtu.be/" << "http://www.youtube.com/embed/";
}


bool YouTubeFilter::filter(QList<HtmlToken> &tokens, QVariantHash /*options*/) const
{
  QList<HtmlToken> out;
  for (int i = 0; i < tokens.size(); i++) {
    HtmlToken token = tokens.at(i);
    if(parse(tokens, out, token.text)) i++;
  }
  tokens += out;
  return !out.isEmpty();
}

bool YouTubeFilter::parse(QList<HtmlToken> &tokens, QList<HtmlToken> &out, const QString &text) const
{
    for(int i = 0; i < m_urls.size(); i++)
    {
        int posx = text.indexOf(m_urls.at(i));
        if(posx != -1)
        {
            int posy = text.indexOf("&amp;", posx);

            if(posy == -1)
                posy = text.indexOf(' ', posx);

            QString link = text.mid(posx, posy - posx);
            QUuid uuid = QUuid::createUuid();
            int posz = link.indexOf(m_urls.at(i));
            link = link.mid(posz+m_urls.at(i).length());
            QString finalLink = QString("<br><a id=\"YouTubePlayerLabel_%1_%2\" href=\"javascript:ShowYouTubePlayer(\'%1\', \'%2\');\">show</a> <div id=\"YouTubePlayer_%1_%2\" style=\"display: none\"><iframe class=\"youtube-player\" type=\"text/html\" width=\"640\" height=\"385\" src=\"http://www.youtube.com/embed/%1\" frameborder=\"0\" onload=\"javascript:GetYouTubeVideoName(\'%1\', \'%2\');\"></iframe></div>").arg(link).arg(uuid.toString());
            qDebug() << finalLink;
            HtmlToken playerToken(HtmlToken::Tag, finalLink);
            out.append(playerToken);

            return true;
        }
    }
    return false;
}
