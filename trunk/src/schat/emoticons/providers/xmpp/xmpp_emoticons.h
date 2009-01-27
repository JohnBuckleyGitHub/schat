/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 * Base class XmppEmoticons
 * Copyright © 2008 by Carlo Segato <brandon.ml@gmail.com>
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

#ifndef XMPP_EMOTICONS_H
#define XMPP_EMOTICONS_H

#include <QtXml/QDomDocument>
#include <QObject>

#include <emoticons/emoticonsprovider.h>

class XmppEmoticons : public EmoticonsProvider
{
  Q_OBJECT

public:
  XmppEmoticons(QObject *parent);
  bool loadTheme(const QString &path);

private:
  QDomDocument m_themeXml;
};

#endif /* XMPP_EMOTICONS_H */

// kate: space-indent on; indent-width 4; replace-tabs on;
